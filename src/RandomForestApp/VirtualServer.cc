#include "VirtualServer.h"
#include <random>
#include <math.h>

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
        computationPower = par("computationPower").intValue();
        penaltyInterval = par("penaltyInterval").doubleValue();
        double expectedReliability = par("expectedReliability").doubleValue();
        timeFailed = 0;
        progress = 0;
        latestWorkTime = 0;
        currentComputationWork = 0;

        taskCounter = -1;
        taskId = 0;

        reliability = normalReliability(expectedReliability);
        EV_WARN << "Virtual Server with id: " << id <<" will have a reliability of :"<<reliability<< endl;

        finishCalled = false;
        hasTask = false;

        scheduleAt(simTime() + beaconInterval+uniform(.2, .4), sendWSMEvt);
    }
}
void VirtualServer::handleSelfMsg(cMessage* msg) {
    perhapsFinish();
    if(simTime().dbl()>penaltyTime){
        if(hasTask){
            progress += computationPower*(simTime().dbl()-latestWorkTime);
            if(progress>=currentComputationWork){
                sendDown(generateTaskCompletion(currentComputationWork,taskCounter));
            }
            else{
                sendDown(generateHeartbeat());
            }
            latestWorkTime = simTime().dbl();
        }
        else{
            sendDown(generateHeartbeat());
        }
    }

    scheduleAt(simTime() + beaconInterval, sendWSMEvt);
}



void VirtualServer::onWSM(WaveShortMessage* wsm) {
   if (TaskRequest* task = dynamic_cast<TaskRequest*>(wsm)) {
        if (loadBalancerId == task->getLoadBalancerId()
                && id == task->getVirtualServerId() && simTime().dbl()>penaltyTime) {
            if(!failed()){
                currentComputationWork = task->getComputationWork();
                taskCounter = task->getTaskCounter();
                taskId = task->getTaskId();
                progress = 0;
                hasTask = true;
                EV_WARN << "Virtual Server with id: " << id <<" received task of load :"<<currentComputationWork
                        <<" for a task id of : "<<task->getTaskId()<< endl;
            }
            else{
                hasTask = false;
            }
        }
    }
}

bool VirtualServer::failed(){
    if (uniform(0, 1) > reliability) {
        double failingPeriod = penaltyInterval* beaconInterval.dbl();
        timeFailed+=failingPeriod;
        penaltyTime = failingPeriod +simTime().dbl();
        EV_WARN << "Virtual Server with id: " << id
                       << " will fail until : "<<penaltyTime << endl;
        latestWorkTime = penaltyTime;
        return true;
    }
    return false;
}


Heartbeat * VirtualServer::generateHeartbeat() {
    Heartbeat* heartbeat = new Heartbeat();
    populateWSM(heartbeat);
    heartbeat->setVirtualServerId(id);
    heartbeat->setComputationPower(computationPower);
    return heartbeat;

}
TaskCompletion * VirtualServer::generateTaskCompletion(double computationWork,int taskCounter) {
    TaskCompletion* taskCompletion = new TaskCompletion();
    populateWSM(taskCompletion);
    taskCompletion->setVirtualServerId(id);
    taskCompletion->setComputationWork(computationWork);
    taskCompletion->setTaskCounter(taskCounter);
    taskCompletion->setComputationPower(computationPower);
    taskCompletion->setTaskId(taskId);
    EV_WARN << "Virtual Server with id: " << id
                   << " sending task completion message for a task load of : "
                   << computationWork <<" for the task id of: "<<
                   taskId<< endl;
    return taskCompletion;
}

double VirtualServer::normalReliability(double expectedReliability){
    //While unlikely the following normal distribution can output values less than 0 and more than 1
    std::normal_distribution<double> distribution(expectedReliability,.2);
    std::default_random_engine generator;
    generator.seed(time(NULL));
    double reliability = distribution(generator);
    if(reliability<0){
        reliability = 0;
    }
    else if(reliability>1){
        reliability = 1;
    }
    return reliability;
}

void VirtualServer::perhapsFinish(){
    if(simTime().dbl()>900 && !finishCalled){
        callFinish();
        finishCalled = true;
    }
}

void VirtualServer::finish(){
    EV_WARN << "Virtual Server with id: " << id<< " time failed "<<timeFailed << endl;
    std::string scTimeFailed = "Micro-cloud :"+ std::to_string(id)+ " failed";
    recordScalar(scTimeFailed.c_str(),timeFailed);
}


