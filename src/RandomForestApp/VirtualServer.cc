#include "VirtualServer.h"
#include <random>

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
        id =  par("id").intValue();
        loadBalancerId = par("loadBalancerId").intValue();
        progress = 0;
        computationPower = par("computationPower").intValue();
        reliability = normalReliability();
        timeOfLatestMessage = -1;
        scheduleAt(simTime() + beaconInterval+uniform(.2, .4), sendWSMEvt);
    }

}
void VirtualServer::handleSelfMsg(cMessage* msg) {
    if (rand() < reliability) {
        currentComputationTask = -1;
    } else {
        progress += computationPower*(simTime().dbl()-timeOfLatestMessage);
        if(currentComputationTask!=-1 && progress>=currentComputationTask){
            sendDown(generateTaskCompletion(currentComputationTask));
        }
        else{
            sendDown(generateHeartbeat());
        }
    }
    timeOfLatestMessage = simTime().dbl();
    scheduleAt(simTime() + beaconInterval+uniform(.2, .4), sendWSMEvt);
}



void VirtualServer::onWSM(WaveShortMessage* wsm) {
   if (TaskRequest* task = dynamic_cast<TaskRequest*>(wsm)) {
        if (loadBalancerId == task->getLoadBalancerId()
                && id == task->getVirtualServerId()) {
            //Note that the task request message is accepted and prioritized regardless
            //of the fact that it might be in the middle of another task.
            //This is because if a load balancer sends a new task before receiving a task completion,
            //it has considered the node as not functioning and invalidated its task/

            currentComputationTask = task->getComputationTask();
            progress = 0;
            EV_WARN << "Virtual Server with id: " << id <<" received task of :"<<currentComputationTask<< endl;
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
    EV_WARN << "Virtual Server with id: " << id
                   << " sending task completion message for a task load of : "<<computationTask << endl;
    return taskCompletion;
}

double VirtualServer::normalReliability(){
    //While unlikely the following normal distribution can output values less than 0 and more than 1
    std::normal_distribution<double> distribution(.6,.2);
    std::default_random_engine generator;
    double reliability = distribution(generator);


    if(reliability<0){
        reliability = 0;
    }
    else if(reliability>1){
        reliability = 1;
    }
    return reliability;
}


