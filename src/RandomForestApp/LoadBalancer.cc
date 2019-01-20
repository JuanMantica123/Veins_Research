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
        id = getId();
        scheduleAt(simTime() + uniform(0, 5), sendWSMEvt);
    }
}

void LoadBalancer::handleSelfMsg(cMessage* msg) {
//    clearFailingMicroClouds();
    if (currentComputationTask <= 0) {
        currentComputationTask = 10;
    }
    distributeWorkLoad();
    scheduleAt(simTime() + 5 + uniform(0, 5), sendWSMEvt);
}

void LoadBalancer::onWSM(WaveShortMessage* wsm) {
    if (ConnectionRequest* request = dynamic_cast<ConnectionRequest*>(wsm)) {
        int virtualServerId = request->getVirtualServerId();
        sendDown(generateConnectionApproval(virtualServerId));
        EV_WARN << "Sending connection approval to: " << virtualServerId
                       << endl;
    } else if (ConnectionConfirmation* confirmation =
            dynamic_cast<ConnectionConfirmation*>(wsm)) {
        if (confirmation->getLoadBalancerId() == id) {
            int virtualServerId = confirmation->getVirtualServerId();
            MicroCloud microcloud;
            microcloud.setLastHeartbeat(simTime().dbl());
            microcloud.setComputationPower(confirmation->getComputationPower());
            microcloud.setIdle(true);
            microcloud.setVirtualServerId(virtualServerId);
            idToMicrocloud[virtualServerId] = microcloud;
            EV_WARN << "Received connection confirmation from: "
                           << virtualServerId << endl;
        }
    } else if (Heartbeat* heartbeat = dynamic_cast<Heartbeat*>(wsm)) {
        int virtualServerId = heartbeat->getVirtualServerId();
        MicroCloud microcloud = idToMicrocloud[virtualServerId];
        microcloud.setLastHeartbeat(simTime().dbl());
        EV_WARN << "Received heart beat from " << virtualServerId << endl;

    } else if (TaskCompletion* completion = dynamic_cast<TaskCompletion*>(wsm)) {
        int virtualServerId = completion->getVirtualServerId();
        double completedComputationTask = completion->getComputationTask();
        currentComputationTask -= completedComputationTask;
        MicroCloud microcloud = idToMicrocloud[virtualServerId];
        microcloud.setIdle(true);
        EV_WARN << "Received task completion message from: " << virtualServerId
                       << " which completed a task of :" <<completedComputationTask<<endl;
    }
}

void LoadBalancer::distributeWorkLoad() {
    double idleComputation = 0;
    std::vector<MicroCloud> idleClouds;
    for (auto const& x : idToMicrocloud) {
        MicroCloud mc = x.second;
        if (mc.isIdle()) {
            idleClouds.push_back(mc);
            idleComputation += mc.getComputationPower();
        }
    }
    for (MicroCloud mc : idleClouds) {
        mc.setIdle(false);
        double computationFraction = mc.getComputationPower() / idleComputation;
        double virtualServertask = computationFraction * currentComputationTask;
        EV_WARN << "Sending computation task of: " << virtualServertask << endl;
        sendDown(
                generateTaskRequest(virtualServertask,
                        mc.getVirtualServerId()));
    }

}

void LoadBalancer::clearFailingMicroClouds() {
    double time = simTime().dbl();
    for (auto const& x : idToMicrocloud) {
        int virtualServerId = x.first;
        MicroCloud mc = x.second;
        if (mc.getLastHeartbeat() + 10 > time) {
            idToMicrocloud.erase(virtualServerId);
        }

    }

}

ConnectionApproval* LoadBalancer::generateConnectionApproval(
        int virtualServerId) {
    ConnectionApproval* approval = new ConnectionApproval();
    populateWSM(approval);
    approval->setVirtualServerId(virtualServerId);
    approval->setLoadBalancerId(id);
    return approval;
}

TaskRequest* LoadBalancer::generateTaskRequest(double virtualServertask,
        int virtualServerId) {
    TaskRequest* request = new TaskRequest();
    populateWSM(request);
    request->setVirtualServerId(virtualServerId);
    request->setComputationTask(virtualServertask);
    request->setLoadBalancerId(id);
    return request;
}

