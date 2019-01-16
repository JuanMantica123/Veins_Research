#include "LoadBalancer.h"

#ifndef DBG_APP
#define EV_WARN
#endif

Define_Module(LoadBalancer);

void LoadBalancer::initialize(int stage) {
    BaseWaveApplLayer::initialize(stage);
    if (stage == 0) {
        int computationPower = 0;
        currentComputationTask = 0;
        sendWSMEvt = new cMessage("wsm evt");
    } else if (stage == 1) {
        id = getId();
    }
    scheduleAt(simTime() + 1 + uniform(0, 5), sendWSMEvt);
}

void LoadBalancer::handleSelfMsg(cMessage* msg) {
    clearFailingMicroClouds();
    if (currentComputationTask <= 0) {
        currentComputationTask = 10;
        distributeWorkLoad();
    }
    scheduleAt(simTime() + 5 + uniform(0, 5), sendWSMEvt);
}

void LoadBalancer::onWSM(WaveShortMessage* wsm) {
    if (ConnectionRequest* request = dynamic_cast<ConnectionRequest*>(wsm)) {
        sendDown(generateConnectionApproval(request->getVirtualServerId()));
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

        }
    } else if (Heartbeat* heartbeat = dynamic_cast<Heartbeat*>(wsm)) {
        int virtualServerId = heartbeat->getVirtualServerId();
        MicroCloud microcloud = idToMicrocloud[virtualServerId];
        microcloud.setLastHeartbeat(simTime().dbl());

    } else if (TaskCompletion* completion = dynamic_cast<TaskCompletion*>(wsm)) {
        int virtualServerId = completion->getVirtualServerId();
        currentComputationTask -= completion->getComputationTask();
        MicroCloud microcloud = idToMicrocloud[virtualServerId];
        microcloud.setIdle(true);
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

