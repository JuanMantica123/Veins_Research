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
        currentComputationTask = 0;
        sendWSMEvt = new cMessage("wsm evt");
    } else if (stage == 1) {
        id = par("id").intValue();
        lowerBound = par("lowerBound").intValue();
        upperBound = par("upperBound").intValue();
        reputationOn = par("reputationOn").boolValue();
        heartBeatTimeout = par("timeOutIntervals").intValue()*beaconInterval.dbl();
        expectedComputationTask = par("expectedComputationTask").doubleValue();
        replicationFactor = par("replicationFactor").doubleValue();
        workFinished = 0;
        //TODO: extend double of progress later to general one
        progress = 0;
        currentComputationTask = 0;
        failedWork = 0;
        taskCompleted = 0;
        taskRate= .1;
        taskCompletedVector.setName("Task Completed");
        workFinishedVector.setName("Work finished");
        taskCompletedVector.recordWithTimestamp(simTime(),taskCompleted);
        workFinishedVector.recordWithTimestamp(simTime(),workFinished);
        finishCalled = false;
        for(int i=lowerBound;i<=upperBound;i++){
            MicroCloud * microcloud = new MicroCloud();
            microcloud->setVirtualServerId(i);
            microcloud->setLastHeartbeat(-heartBeatTimeout);
            microcloud->setReputation(1);
            microcloud->setComputationPower(-1);
            microcloud->setLatestComputationTask(0);
            microcloud->setIdle(true);
            microcloud->setFunctioning(true);
            microcloud->setTaskCounter(0);
            microcloud->setWorkDone(0);
            microcloud->setLatestTaskReplicated(false);
            idToMicrocloud[i] = microcloud;
        }
        scheduleAt(simTime() +  beaconInterval+uniform(.2, .4), sendWSMEvt);
    }
}

void LoadBalancer::handleSelfMsg(cMessage* msg) {
    if(simTime().dbl()>900 && !finishCalled){
        callFinish();
        finishCalled = true;
    }

    if(uniform(0, 1))
    if (progress >= currentComputationTask) {
        workFinished+=currentComputationTask;
        taskCompleted++;
        if(!finishCalled){
            taskCompletedVector.recordWithTimestamp(simTime(),taskCompleted);
            workFinishedVector.recordWithTimestamp(simTime(),workFinished);
        }


        //Absolute value in the extremely unlikely case the value from the normal distribution is negative
        currentComputationTask = abs(normal(expectedComputationTask-1, 2, 0))+1;
        //TODO: Should use statistics instead of logs to record this
        progress = 0;
        EV_WARN << "New task: " << currentComputationTask <<" total work finished: "<<workFinished<<endl;
        distributeWorkLoad(currentComputationTask);

    }
    else{
        failedWork+= clearFailingMicroClouds();
        if(isAnMCIdle() && failedWork>0){
            EV_WARN << "Distributing failed work of: " <<failedWork<<endl;
            distributeWorkLoad(failedWork);
            failedWork = 0;
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
        EV_WARN << "Received heart beat from " << virtualServerId << endl;

    } else if (TaskCompletion* completion = dynamic_cast<TaskCompletion*>(wsm)) {
        int virtualServerId = completion->getVirtualServerId();
        MicroCloud * microcloud = idToMicrocloud[virtualServerId];

        microcloud->setLastHeartbeat(simTime().dbl());
        microcloud->setIdle(true);
        microcloud->setFunctioning(true);

        //The counter is used to handle failures and make sure that no task completion is double counted
        int counter = completion->getTaskCounter();
         if(microcloud->getTaskCounter()==counter){
            double completedComputationTask = completion->getComputationTask();
            progress += completedComputationTask;
            microcloud->setWorkDone(microcloud->getWorkDone()+completedComputationTask);
            microcloud->setTaskCounter(microcloud->getTaskCounter()+1);
            if(reputationOn){
                microcloud ->setReputation(microcloud->getReputation()+.05);
                EV_WARN << "Received task completion message from: " << virtualServerId
                               << " which completed a task of :" <<completedComputationTask<<" ,new reputation is : "
                               <<microcloud->getReputation()<<endl;
            }
            else{
                EV_WARN << "Received task completion message from: " << virtualServerId
                               << " which completed a task of :" <<completedComputationTask<<endl;
            }
        }
    }
}

void LoadBalancer::distributeWorkLoad(double computationTask) {
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
        double virtualServerTask;
        if(idleClouds.size()>1){
            virtualServerTask = replicationFactor*computationFraction * computationTask;
            mc->setLatestTaskReplicated(true);
        }
        else{
            virtualServerTask = computationFraction * computationTask;
            mc->setLatestTaskReplicated(false);
        }
        mc->setLatestComputationTask(virtualServerTask);
        EV_WARN << "Sending computation task of: " << mc->getLatestComputationTask() <<" to: "<<mc->getVirtualServerId()<< endl;
        sendDown(
                generateTaskRequest(virtualServerTask,
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
//Loops over the micro-clouds and determines the ones that have timed out and returns the task they failed
double LoadBalancer::clearFailingMicroClouds() {
    double failedTask = 0;
    for (auto const& x : idToMicrocloud) {
        MicroCloud * mc = x.second;
        if (mc->getLastHeartbeat() + heartBeatTimeout  < simTime().dbl() && mc->isFunctioning()) {
            mc->setFunctioning(false);
            if(reputationOn){
                mc->setReputation(std::max(mc->getReputation()-.05,0.0));
                EV_WARN << "Microcloud: " <<mc->getVirtualServerId()<<" with task "<<mc->getLatestComputationTask()
                        << " failed"<< " ,new reputation is: "<<mc->getReputation()<<  endl;
            }
            else{
                EV_WARN << "Microcloud: " <<mc->getVirtualServerId()<<" with task "<<mc->getLatestComputationTask()<<" failed"<<endl;

            }
            if(mc->isLatestTaskReplicated()){
                failedTask+= mc->getLatestComputationTask()/replicationFactor;
                mc->setLatestTaskReplicated(false);
            }
            else{
                failedTask+=mc->getLatestComputationTask();
            }
            mc->setLatestComputationTask(0);
            mc->setTaskCounter(mc->getTaskCounter()+1);

        }

    }
    return failedTask;

}

TaskRequest* LoadBalancer::generateTaskRequest(double virtualServertask,
        int virtualServerId,int counter) {
    TaskRequest* request = new TaskRequest();
    populateWSM(request);
    request->setVirtualServerId(virtualServerId);
    request->setComputationTask(virtualServertask);
    request->setLoadBalancerId(id);
    request->setTaskCounter(counter);
    return request;
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
    recordScalar(scTask.c_str(),taskCompleted);
    EV_WARN << "Load balancer : " <<id<<" finished a total work of: "<<workFinished<<" and completed "<<taskCompleted<<" task"<<endl;
}

