#include "LoadBalancer.h"

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
        heartBeatTimeout = 3 *beaconInterval.dbl();
        failedWork = 0;
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
            idToMicrocloud[i] = microcloud;
        }
        scheduleAt(simTime() +  beaconInterval+uniform(.2, .4), sendWSMEvt);
    }
}

void LoadBalancer::handleSelfMsg(cMessage* msg) {
    for (auto const& x : idToMicrocloud) {
        MicroCloud * mc = x.second;
    }
    if (currentComputationTask <= 0) {
        //Absolute value in the extremely unlikely case the value from the normal distribution is negative
        currentComputationTask = abs(normal(5, 2, 0));
        EV_WARN << "New task: " << currentComputationTask << endl;
        distributeWorkLoad(currentComputationTask);

    }
    else{
        failedWork+= clearFailingMicroClouds();
        if(isAnMCIdle() && failedWork>0){
            distributeWorkLoad(failedWork);
            failedWork = 0;
        }
    }
    scheduleAt(simTime() +  beaconInterval+uniform(.2, .4), sendWSMEvt);
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
        double completedComputationTask = completion->getComputationTask();
        if(microcloud->getTaskCounter()==counter){
            currentComputationTask -= completedComputationTask;
            microcloud->setTaskCounter(microcloud->getTaskCounter()+1);
            EV_WARN << "Received task completion message from: " << virtualServerId
                           << " which completed a task of :" <<completedComputationTask<<endl;
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
            idleComputation += mc->getComputationPower();
        }
    }
    for (MicroCloud * mc : idleClouds) {
        mc->setIdle(false);
        double computationFraction = mc->getComputationPower() / idleComputation;
        double virtualServertask = computationFraction * computationTask;
        mc->setLatestComputationTask(virtualServertask);
        EV_WARN << "Sending computation task of: " << mc->getLatestComputationTask() <<" to: "<<mc->getVirtualServerId()<< endl;
        sendDown(
                generateTaskRequest(virtualServertask,
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


double LoadBalancer::clearFailingMicroClouds() {
    double failedTask = 0;
    for (auto const& x : idToMicrocloud) {
        MicroCloud * mc = x.second;
        if (mc->getLastHeartbeat() + heartBeatTimeout  < simTime().dbl() && mc->isFunctioning()) {
            mc->setFunctioning(false);
            EV_WARN << "Microcloud: " <<mc->getVirtualServerId()<<" with task "<<mc->getLatestComputationTask()
                    << " failed"<< endl;
            failedTask+= mc->getLatestComputationTask();
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

