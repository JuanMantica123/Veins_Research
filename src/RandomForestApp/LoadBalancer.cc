#include "LoadBalancer.h"
#include <iostream>
#include <string>

#ifndef DBG_APP
#define EV_WARN
#endif

Define_Module(LoadBalancer);

void LoadBalancer::initialize(int stage) {
    BaseWaveApplLayer::initialize(stage);
    if (stage == 0) {
        sendWSMEvt = new cMessage("wsm evt");
    } else if (stage == 1) {

        id = par("id").intValue();
        lowerBound = par("lowerBound").intValue();
        upperBound = par("upperBound").intValue();
        reputationOn = par("reputationOn").boolValue();
        heartBeatTimeout = par("timeOutIntervals").intValue()*beaconInterval.dbl();
        expectedComputationWork = par("expectedComputationWork").doubleValue();
        replicationFactor = par("replicationFactor").doubleValue();

        workFinished = 0;
        progress = 0;
        currentComputationWork = 0;
        failedWork = 0;
        taskId = 0;

        taskCompletedVector.setName("Task Completed");
        workFinishedVector.setName("Work finished");
        taskCompletedVector.recordWithTimestamp(simTime(),taskId);
        workFinishedVector.recordWithTimestamp(simTime(),workFinished);

        finishCalled = false;
        sentFirstTask = false;

        for(int i=lowerBound;i<=upperBound;i++){
            MicroCloud * microcloud = new MicroCloud();
            microcloud->setVirtualServerId(i);
            microcloud->setLastHeartbeat(-heartBeatTimeout);
            microcloud->setReputation(1);
            microcloud->setComputationPower(1);
            microcloud->setLatestComputationWork(0);
            microcloud->setIdle(false);
            microcloud->setFunctioning(false);
            microcloud->initializeCounter();
            microcloud->setWorkDone(0);
            microcloud->setLatestTaskReplicated(false);

            idToMicrocloud[i] = microcloud;
        }

        scheduleAt(simTime() +  beaconInterval+uniform(.2, .4), sendWSMEvt);
    }
}

void LoadBalancer::handleSelfMsg(cMessage* msg) {
    perhapsFinish();
    if(!sentFirstTask){
        sendFirstTask();
    }
    else{
        if (progress >= currentComputationWork) {
            //Note that if progress >= currentComputationWork than we also know that we received a completion message
            //from a virtual server and that at least one virtual server is idle
            workFinished+=currentComputationWork;
            taskId+=1;
            currentComputationWork = expectedComputationWork;

            if(!finishCalled){
                taskCompletedVector.recordWithTimestamp(simTime(),taskId);
                workFinishedVector.recordWithTimestamp(simTime(),workFinished);
            }

            progress = 0;
            EV_WARN << "New task of load :" << currentComputationWork <<" total work finished: "<<workFinished<<endl;
            distributeWorkLoad(currentComputationWork);

        }
        else{
            failedWork+= clearFailingMicroClouds();
             if(failedWork>0 && isAnMCIdle()){
                EV_WARN << "Distributing failed work of: " <<failedWork<<endl;
                distributeWorkLoad(failedWork);
                failedWork = 0;
            }
        }
    }
    scheduleAt(simTime() +  beaconInterval, sendWSMEvt);
}

void LoadBalancer::onWSM(WaveShortMessage* wsm) {
    if (Heartbeat* heartbeat = dynamic_cast<Heartbeat*>(wsm)) {

        int virtualServerId = heartbeat->getVirtualServerId();
        MicroCloud * microcloud = idToMicrocloud[virtualServerId];
        if(!microcloud->isFunctioning()){
            microcloud->setFunctioning(true);
            microcloud->setIdle(true);
        }
        microcloud->setLastHeartbeat(simTime().dbl());
        microcloud->setComputationPower(heartbeat->getComputationPower());
        EV_WARN << "Received heart beat from " << virtualServerId << endl;


    } else if (TaskCompletion* completion = dynamic_cast<TaskCompletion*>(wsm)) {

        int virtualServerId = completion->getVirtualServerId();
        MicroCloud * microcloud = idToMicrocloud[virtualServerId];

        microcloud->setLastHeartbeat(simTime().dbl());
        microcloud->setIdle(true);
        microcloud->setFunctioning(true);
        microcloud->setComputationPower(completion->getComputationPower());


        //This taskCounter is used to handle failures and make sure that no task completion is not double counted
        int taskCounter = completion->getTaskCounter();
         if(microcloud->getTaskCounter()==taskCounter){
            double completedComputationWork = completion->getComputationWork();
            microcloud->incrementCounter();
            microcloud->setWorkDone(microcloud->getWorkDone()+completedComputationWork);
            //This taskId tag is to make sure that this work is not for a previous task
            if(completion->getTaskId()==taskId){
                progress += completedComputationWork;
                if(reputationOn){
                    microcloud ->setReputation(microcloud->getReputation()+.05);
                    EV_WARN << "Received task completion message from: " << virtualServerId
                                   << " which completed a task of load :" <<completedComputationWork<<" ,new reputation is : "
                                   <<microcloud->getReputation()<<endl;
                }
                else{
                    EV_WARN << "Received task completion message from: " << virtualServerId
                                   << " which completed a task of load :" <<completedComputationWork<<endl;
                }
            }

         }
    }
}

void LoadBalancer::distributeWorkLoad(double computationWork) {
    double idleComputation = 0;
    std::vector<MicroCloud *> idleClouds;
    for (auto const& x : idToMicrocloud) {
        MicroCloud * mc = x.second;
        if (mc->isFunctioning() && mc->isIdle()) {
            idleClouds.push_back(mc);
            if(reputationOn){
                idleComputation += mc->getComputationPower()*mc->getReputation();
            }
            else{
                idleComputation += mc->getComputationPower();
            }
        }
    }
    for (MicroCloud * mc : idleClouds) {
        mc->setIdle(false);
        double computationFraction;
        if(reputationOn){
            computationFraction = mc->getComputationPower()*mc->getReputation()/idleComputation;
        }
        else{
            computationFraction = mc->getComputationPower()/idleComputation;
        }

        double virtualServerWork;
        if(idleClouds.size()>1){
            virtualServerWork = replicationFactor*computationFraction * computationWork;
            mc->setLatestTaskReplicated(true);
        }
        else{
            virtualServerWork = computationFraction * computationWork;
            mc->setLatestTaskReplicated(false);
        }

        mc->setLatestComputationWork(virtualServerWork);
        mc->setTaskId(taskId);
        EV_WARN << "Sending computation task with load of: " << mc->getLatestComputationWork() <<" to: "<<mc->getVirtualServerId()
                << " with task id of : "<<taskId<<endl;

        sendDown(
                generateTaskRequest(virtualServerWork,
                        mc->getVirtualServerId(),mc->getTaskCounter()));
    }

}

bool LoadBalancer::isAnMCIdle(){
    for (auto const& x : idToMicrocloud) {
        MicroCloud * mc = x.second;
        if (mc->isFunctioning() && mc->isIdle()) {
            return true;
        }
    }
    return false;
}

//Loops over the micro-clouds and determines the ones that have timed out and returns the total work they failed
double LoadBalancer::clearFailingMicroClouds() {
    double failedWork = 0;
    for (auto const& x : idToMicrocloud) {
        MicroCloud * mc = x.second;
        if (mc->getLastHeartbeat() + heartBeatTimeout  < simTime().dbl() && mc->isFunctioning()) {
            mc->setFunctioning(false);
            if(reputationOn){
                mc->setReputation(std::max(mc->getReputation()-.05,0.0));
                EV_WARN << "Microcloud: " <<mc->getVirtualServerId()<<" with task load of :"<<mc->getLatestComputationWork()
                        << " failed"<< " ,new reputation is: "<<mc->getReputation()<<  endl;
            }
            else{
                EV_WARN << "Microcloud: " <<mc->getVirtualServerId()<<" with task load of:  "<<mc->getLatestComputationWork()<<" failed"<<endl;

            }
            if(mc->getTaskId()==taskId){
                if(mc->isLatestTaskReplicated()){
                    failedWork+= mc->getLatestComputationWork()/replicationFactor;
                }
                else{
                    failedWork+=mc->getLatestComputationWork();
                }
            }
            mc->setLatestComputationWork(0);
            mc->incrementCounter();

        }

    }
    return failedWork;

}

void LoadBalancer::sendFirstTask(){
    if(isAnMCIdle()){
        currentComputationWork = expectedComputationWork;
        distributeWorkLoad(currentComputationWork);
        sentFirstTask=true;
        EV_WARN << "First task sent with a load of :" << currentComputationWork <<" total work finished: "<<workFinished<<endl;
    }

}


TaskRequest* LoadBalancer::generateTaskRequest(double virtaulServerWork,
        int virtualServerId,int taskCounter) {
    TaskRequest* request = new TaskRequest();
    populateWSM(request);
    request->setTaskId(taskId);
    request->setVirtualServerId(virtualServerId);
    request->setComputationWork(virtaulServerWork);
    request->setLoadBalancerId(id);
    request->setTaskCounter(taskCounter);
    return request;
}

void LoadBalancer::perhapsFinish(){
    if(simTime().dbl()>900 && !finishCalled){
        callFinish();
        finishCalled = true;
    }
}

void LoadBalancer::finish(){
    for (auto const& x : idToMicrocloud) {
        MicroCloud * mc = x.second;
        int mcId = mc->getVirtualServerId();
        double mcWorkDone = mc->getWorkDone();
        EV_WARN << "Micro-cloud : " <<mcId<<" finished a total work of: "<<mcWorkDone<<endl;
        std::string sc = "Micro-cloud : "+std::to_string(mcId)+" work finished";
        recordScalar(sc.c_str(), mcWorkDone);
    }
    std::string scWork = "Load Balancer : "+std::to_string(id)+" work finished";
    recordScalar(scWork.c_str(),workFinished);
    std::string scTask = "Load Balancer : "+std::to_string(id)+" task finished";
    recordScalar(scTask.c_str(),taskId);
    EV_WARN << "Load balancer : " <<id<<" finished a total work of: "<<workFinished<<" and completed "<<taskId<<" task"<<endl;
}

