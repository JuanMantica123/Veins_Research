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

        id = par("id").longValue();
        lowerBound = par("lowerBound").longValue();
        upperBound = par("upperBound").longValue();
        reputationOn = par("reputationOn").boolValue();
        heartBeatTimeout = par("timeOutIntervals").longValue()*beaconInterval.dbl();
        expectedComputationWork = par("expectedComputationWork").doubleValue();
        replicationFactor = par("replicationFactor").doubleValue();

        workFinished = 0;
        failedWork = 0;
        desiredNumTask = 2;

        workFinishedVector.setName("Work finished");
        workFinishedVector.recordWithTimestamp(simTime(),workFinished);


        for(int i=lowerBound;i<=upperBound;i++){
            MicroCloud * microcloud = new MicroCloud();
            microcloud->setVirtualServerId(i);
            microcloud->setLastHeartbeat(-heartBeatTimeout);
            microcloud->setReputation(1);
            microcloud->setComputationPower(1);
            microcloud->setFunctioning(false);
            microcloud->setWorkDone(0);

            idToMicrocloud[i] = microcloud;
        }

        for(int taskId = 50000; taskId>0;--taskId)
            createNewTask(taskId);

        scheduleAt(simTime() +  beaconInterval+uniform(.2, .4), sendWSMEvt);
    }
}



void LoadBalancer::handleSelfMsg(cMessage* msg) {
    clearFailingMcs();
    sendTasks();
    scheduleAt(simTime() +  beaconInterval, sendWSMEvt);
}

void LoadBalancer::onWSM(WaveShortMessage* wsm) {
    if (Heartbeat* heartbeat = dynamic_cast<Heartbeat*>(wsm)) {

        int virtualServerId = heartbeat->getVirtualServerId();
        MicroCloud * microcloud = idToMicrocloud[virtualServerId];

        microcloud->setFunctioning(true);
        microcloud->setLastHeartbeat(simTime().dbl());
        microcloud->setComputationPower(heartbeat->getComputationPower());

        EV_WARN << "Received heart beat from " << virtualServerId << endl;


    } else if (TaskCompletion* completion = dynamic_cast<TaskCompletion*>(wsm)) {

        int virtualServerId = completion->getVirtualServerId();
        MicroCloud * microcloud = idToMicrocloud[virtualServerId];

        microcloud->setLastHeartbeat(simTime().dbl());
        microcloud->setFunctioning(true);
        microcloud->setComputationPower(completion->getComputationPower());

        int taskId = microcloud->getLatestTaskId();
//        assert(completion->getTaskId()==taskId);

        sendDown(generateTaskAlreadyFinished(taskId));
        deleteTaskFromMCs(taskId);

        double completedComputationWork = completion->getComputationWork();
        microcloud->incrementWorkDone(completedComputationWork);
        workFinished+=completedComputationWork;
        workFinishedVector.recordWithTimestamp(simTime(),workFinished);


        if(reputationOn){
            microcloud ->changeReputation(.05);
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

void LoadBalancer::deleteTaskFromMCs(int taskId){
    std::set<MicroCloud *> mcs = taskIdToMcs[taskId];
    for(auto mc: mcs){
        mc->deleteTask(taskId);
    }
    mcs.clear();
}

void LoadBalancer::sendTasks() {
    int taskToSend = 0;
    std::set<MicroCloud *,MC_Compare_Work> functioningClouds;
    for (auto const& x : idToMicrocloud) {
        MicroCloud * mc = x.second;
        if (mc->isFunctioning()) {
            functioningClouds.insert(mc);
            int taskDeficit = desiredNumTask-mc->getTasks().size();
            taskToSend+=std::max(0,taskDeficit);
        }
    }
    EV_WARN<<"Number of functioning clouds : "<<functioningClouds.size()<<endl;
    while(taskToSend>0){
         taskToSend -= sendTask(functioningClouds);
    }
}

void LoadBalancer::clearFailingMcs() {
    for (auto const& x : idToMicrocloud) {
        MicroCloud * mc = x.second;
        if (mc->getLastHeartbeat() + heartBeatTimeout  < simTime().dbl() && mc->isFunctioning()) {
            mc->setFunctioning(false);
            if(reputationOn){
                mc->setReputation(std::max(mc->getReputation()-.05,0.0));
                EV_WARN << "Microcloud: " <<mc->getVirtualServerId()<<" with task load of :"<<mc->getCurrentWorkLoad()
                        << " failed"<< " ,new reputation is: "<<mc->getReputation()<<  endl;
            }
            else{
                EV_WARN << "Microcloud: " <<mc->getVirtualServerId()<<" with task load of:  "<<mc->getCurrentWorkLoad()<<" failed"<<endl;

            }
            perhapsAddFailedTask(mc->getTasks(),mc->getVirtualServerId());
            mc->recordFailure();
        }

    }

}

int LoadBalancer::sendTask(std::set<MicroCloud *,MC_Compare_Work> functioningClouds){
    std::set<MicroCloud *> cloudsReplicatedTo;
    auto taskIterator = tasks.begin();
    Task * task = *taskIterator;
    tasks.erase(taskIterator);
    while(functioningClouds.size()>0 && cloudsReplicatedTo.size()<replicationFactor){
        auto mcIterator = functioningClouds.begin();
        MicroCloud * mostIdleCloud = *mcIterator;
        cloudsReplicatedTo.insert(mostIdleCloud);
        functioningClouds.erase(mcIterator);
        mostIdleCloud->pushTask(task);
        sendDown(generateTaskRequest(task->getWorkLoad(), mostIdleCloud->getVirtualServerId(),task->getId()));
    }
    //Resorting MCs in functioning clouds
    for(auto mc: cloudsReplicatedTo){
        functioningClouds.insert(mc);
    }
    taskIdToMcs[task->getId()]=cloudsReplicatedTo;
    return cloudsReplicatedTo.size();


}
void LoadBalancer::createNewTask(int taskId) {
    Task * newTask = new Task(taskId, expectedComputationWork);
    tasks.insert(newTask);

}

void LoadBalancer::perhapsAddFailedTask(std::deque<Task *> failedTasks, int mcId){
    while(!failedTasks.empty()){
        Task * task = failedTasks.front();
        failedTasks.pop_front();
        std::set<MicroCloud *> mcs = taskIdToMcs[task->getId()];
        for (auto mcIterator = mcs.begin(); mcIterator != mcs.end(); ++mcIterator) {
                    MicroCloud * iteratingMc = *mcIterator;
                    if (iteratingMc->getVirtualServerId() == mcId) {
                        mcs.erase(mcIterator);
                        EV_WARN<<"Deleting microcloud : "
                                <<iteratingMc->getVirtualServerId()
                                <<" from task : "<< task->getId()
                                <<" , number of mcs handling that task : "<< mcs.size()<<endl;
                        break;
                    }
                }
        if(mcs.empty()){
            EV_WARN<<"Reinserting failed task : "<<task->getId()<<endl;
            tasks.insert(task);
        }
    }
}

TaskRequest* LoadBalancer::generateTaskRequest(double virtualServerWork,
        int virtualServerId,int taskId) {
    TaskRequest* request = new TaskRequest();
    populateWSM(request);
    request->setTaskId(taskId);
    request->setVirtualServerId(virtualServerId);
    request->setComputationWork(virtualServerWork);
    request->setLoadBalancerId(id);
    EV_WARN << "Sending task request to "<< request->getVirtualServerId()
            <<" with a work load of "<<request->getComputationWork()<<" and a task id of "<<request->getTaskId()<<endl;
    return request;
}

TaskAlreadyFinished * LoadBalancer::generateTaskAlreadyFinished(int taskId){
    TaskAlreadyFinished* finished = new TaskAlreadyFinished();
    populateWSM(finished);
    finished->setTaskId(taskId);
    finished->setLoadBalancerId(id);
    return finished;
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
    EV_WARN << "Load balancer : " <<id<<" finished a total work of: "<<workFinished<<endl;
}

