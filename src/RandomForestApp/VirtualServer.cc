#include "VirtualServer.h"

#ifndef DBG_APP
#define EV_WARN
#endif

Define_Module(VirtualServer);

void VirtualServer::initialize(int stage) {
    BaseWaveApplLayer::initialize(stage);
    if (stage == 0) {
        sendWSMEvt = new cMessage("wsm evt");
    }
    if (stage == 1) {
        id = getId();
        loadBalancerId = -1;
        progress = 0;
        computationPower = par("computationPower").intValue();
        reliability = par("reliability").intValue();
        sendDelayedDown(generateConnectionRequest(), uniform(0, 5));
        scheduleAt(simTime() + beaconInterval + uniform(0, 5), sendWSMEvt);
    }

}
void VirtualServer::handleSelfMsg(cMessage* msg) {
    if (rand() < reliability) {
        currentComputationTask = -1;
        progress = 0;
        loadBalancerId = -1;
    } else {
        if (loadBalancerId == -1) {
            sendDown(generateConnectionRequest());
        } else {
            progress += computationPower * beaconInterval.dbl();
            if (progress >= currentComputationTask) {
                sendDown(generateTaskCompletion(currentComputationTask));
            } else {
                sendDown(generateHeartbeat());
            }
        }
        scheduleAt(simTime() + beaconInterval, msg);

    }
}

void VirtualServer::onWSM(WaveShortMessage* wsm) {
    if (ConnectionApproval* approval = dynamic_cast<ConnectionApproval*>(wsm)) {
        if(loadBalancerId==-1){
            int requestingLoadBalancerId = approval->getLoadBalancerId();
            sendDown(generateConnectionConfirmation(requestingLoadBalancerId));
        }
    }
    else if(ConnectionConfirmation* confirmation= dynamic_cast<ConnectionConfirmation*>(wsm) ){
        loadBalancerId = confirmation->getLoadBalancerId();
    }
    else if (TaskRequest* task = dynamic_cast<TaskRequest*>(wsm)){
        if(loadBalancerId==task->getLoadBalancerId() && id == task->getVirtualServerId() && currentComputationTask<=0){
            currentComputationTask = task->getComputationTask();
        }
    }
}

Heartbeat * VirtualServer::generateHeartbeat() {
    Heartbeat* heartbeat = new Heartbeat();
    populateWSM(heartbeat);
    heartbeat->setVirtualServerId(id);
    return heartbeat;

}
TaskCompletion * VirtualServer::generateTaskCompletion(double computationTask) {
    TaskCompletion* taskCompletion = new TaskCompletion();
    populateWSM(taskCompletion);
    taskCompletion->setVirtualServerId(id);
    taskCompletion->setComputationTask(computationTask);
    return taskCompletion;
}
ConnectionRequest* VirtualServer::generateConnectionRequest() {
    ConnectionRequest * connectionRequest = new ConnectionRequest();
    populateWSM(connectionRequest);
    connectionRequest->setVirtualServerId(id);
    return connectionRequest;
}
ConnectionConfirmation * VirtualServer::generateConnectionConfirmation(int loadBalancerId){
    ConnectionConfirmation * connectionConfirmation = new ConnectionConfirmation();
    populateWSM(connectionConfirmation);
    connectionConfirmation->setVirtualServerId(id);
    connectionConfirmation->setLoadBalancerId(loadBalancerId);
    return connectionConfirmation;


}

