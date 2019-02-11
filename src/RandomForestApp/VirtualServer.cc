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
        expectedReliability = par("expectedReliability").doubleValue();
        timeFailed = 0;
        progress = 0;
        latestWorkTime = 0;
        currentComputationTask = 0;
        taskCounter = -1;
        double reliability;
        //TODO: Change this from debugging to real
        // = reliability = normalReliability(expectedReliability);
        if(id==0){
            reliability =.4;
        }
        else{
            reliability = .9;
        }
        //This equation is used to take into account the length of the penalty interval.
        // For example: if the reliability of a node is 50% we would still want it to be 50% regardless
        // that a failure may take 5 or 10 seconds
        calculatedReliability  = .5;

                //pow(reliability,(1/penaltyInterval));
        EV_WARN << "Virtual Server with id: " << id <<" will have a reliability of :"<<calculatedReliability<< endl;
        finishCalled = false;
        scheduleAt(simTime() + beaconInterval+uniform(.2, .4), sendWSMEvt);
    }
}
void VirtualServer::handleSelfMsg(cMessage* msg) {
    if(simTime().dbl()>900 && !finishCalled){
        callFinish();
        finishCalled = true;
    }
    if(simTime().dbl()>penaltyTime){
        if (uniform(0, 1) > calculatedReliability) {
            double failingPeriod = penaltyInterval* beaconInterval.dbl();
            timeFailed+=failingPeriod;
            penaltyTime = failingPeriod +simTime().dbl();
            EV_WARN << "Virtual Server with id: " << id
                           << " will fail until : "<<penaltyTime << endl;
            latestWorkTime = penaltyTime;
        }
        else{
            progress += computationPower*(simTime().dbl()-latestWorkTime);
            if(progress>=currentComputationTask && taskCounter!=-1){
                sendDown(generateTaskCompletion(currentComputationTask,taskCounter));
            }
            else{
                sendDown(generateHeartbeat());
            }
            latestWorkTime = simTime().dbl();
        }
    }
    scheduleAt(simTime() + beaconInterval, sendWSMEvt);
}



void VirtualServer::onWSM(WaveShortMessage* wsm) {
   if (TaskRequest* task = dynamic_cast<TaskRequest*>(wsm)) {
        if (loadBalancerId == task->getLoadBalancerId()
                && id == task->getVirtualServerId() && simTime().dbl()>penaltyTime) {
            //Note that the task request message is accepted and prioritized regardless
            //of the fact that it might be in the middle of another task.
            //This is because if a load balancer sends a new task before receiving a task completion,
            //it has considered the node as not functioning and invalidated its task/

            currentComputationTask = task->getComputationTask();
            taskCounter = task->getTaskCounter();
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
TaskCompletion * VirtualServer::generateTaskCompletion(double computationTask,int taskCounter) {
    TaskCompletion* taskCompletion = new TaskCompletion();
    populateWSM(taskCompletion);
    taskCompletion->setVirtualServerId(id);
    taskCompletion->setComputationTask(computationTask);
    taskCompletion->setTaskCounter(taskCounter);
    EV_WARN << "Virtual Server with id: " << id
                   << " sending task completion message for a task of : "<<computationTask << endl;
    return taskCompletion;
}

double VirtualServer::normalReliability(double expectedReliability){
    //While unlikely the following normal distribution can output values less than 0 and more than 1
    std::normal_distribution<double> distribution(expectedReliability,.1);
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
void VirtualServer::finish(){
    EV_WARN << "Virtual Server with id: " << id<< " time failed "<<timeFailed << endl;
    std::string scTimeFailed = "Micro-cloud :"+ std::to_string(id)+ " failed";
    recordScalar(scTimeFailed.c_str(),timeFailed);
}


