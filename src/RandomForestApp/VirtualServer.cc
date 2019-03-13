#include "VirtualServer.h"
#include <random>
#include <math.h>
#include "csv.h"

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
        computationPower = 0;
        //computationPower = par("computationPower").doubleValue();
        penaltyInterval = par("penaltyInterval").doubleValue();
        reliability = par("expectedReliability").doubleValue();
        minNumCars = par("minNumCars").intValue();

        latestTimeFailedCalled = 0;
        timeFailed = 0;
        latestWorkTime = 0;
        penaltyTime = 0;
        currentTask = NULL;


        std::string configFolder = "/home/pedro/Documents/juan_work/Veins_Research/src/RandomForestApp/VirtualServerConfig/";

        io::CSVReader<1> in(configFolder+"lb0_server_power.csv");
        in.read_header(io::ignore_extra_column, std::to_string(id));
        double computationPower; int time = 0;
        while(in.read_row(computationPower)){
            timeToComputationPower[time] = computationPower;
            time++;
        }

        io::CSVReader<1> inCars(configFolder+"lb0_virtual_servers.csv");
            inCars.read_header(io::ignore_extra_column, std::to_string(id));
                int numCars; time = 0;
                while(inCars.read_row(numCars)){
                    timetoNumCars[time] = numCars;
                    time++;
                }

       scheduleAt(simTime() + beaconInterval+uniform(.2, .4), sendWSMEvt);
    }
}

void VirtualServer::handleSelfMsg(cMessage* msg) {
    int time = (int)simTime().dbl();
    if (simTime().dbl() > penaltyTime  && !failed(time)) {
        if(id==6){
            EV_WARN<<"I am alive"<<computationPower<<endl;
        }
        if (currentTask != NULL) {
            computationPower = timeToComputationPower[time];
            double latestProgress = computationPower * (simTime().dbl() - latestWorkTime);
            currentTask->incrementProgress(latestProgress);
            if (currentTask->getProgress() >= currentTask->getWorkLoad()) {
                sendDown(generateTaskCompletion(currentTask->getWorkLoad(), currentTask->getId()));
                loadNewTask(currentTask->getProgress()-currentTask->getWorkLoad());
            } else {
                sendDown(generateHeartbeat());
            }
        } else {
            if (currentTask == NULL && !tasks.empty()) {
                loadNewTask(0);
            }
            sendDown(generateHeartbeat());
        }
    }
    latestWorkTime = simTime().dbl();
    scheduleAt(simTime() + beaconInterval, sendWSMEvt);
}


void VirtualServer::onWSM(WaveShortMessage* wsm) {
   if (TaskRequest* task = dynamic_cast<TaskRequest*>(wsm)) {
        if (loadBalancerId == task->getLoadBalancerId()
                && id == task->getVirtualServerId() && simTime().dbl()>penaltyTime) {

            Task * newTask = new Task(task->getTaskId(),task->getComputationWork());
            tasks.push_back(newTask);


        }
    }
   else if (TaskAlreadyFinished* finishedTask = dynamic_cast<TaskAlreadyFinished*>(wsm)) {
        if (loadBalancerId == finishedTask->getLoadBalancerId()) {
            deleteTask(finishedTask->getTaskId());
        }
    }
}

void VirtualServer::deleteTask(int taskId){
    for (auto task = tasks.begin(); task != tasks.end(); ++task) {
        Task * iteratingTask = *task;
        if (iteratingTask->getId() == taskId) {
            EV_WARN<<"Virtual Server : "<<id<<"  will delete task "<<taskId<<endl;
            tasks.erase(task);
            break;
        }
    }
}

bool VirtualServer::failed(int time){
    int numCars = timetoNumCars[time];
    if(numCars<minNumCars){
        fail();
        return true;
    }
    else if(latestTimeFailedCalled+penaltyInterval<simTime().dbl()){
        latestTimeFailedCalled = simTime().dbl();
        if (((double) rand() / (RAND_MAX)) > reliability) {
            fail();
            return true;
        }
    }
    return false;
}

void VirtualServer::fail(){
    timeFailed+=penaltyInterval;
    penaltyTime = penaltyInterval +simTime().dbl();
    EV_WARN << "Virtual Server with id: " << id
                   << " will fail until : "<<penaltyTime << endl;
    latestWorkTime = penaltyTime;
    tasks.clear();
}

void VirtualServer::loadNewTask(double unacountedProgress){
    if (!tasks.empty()) {
        currentTask =tasks.front();
        currentTask->incrementProgress(unacountedProgress);
        tasks.pop_front();
    } else {
        currentTask = NULL;
    }
}

void VirtualServer::refreshDisplay() const
{
    std::string display = "Task : ";
    for(auto task : tasks){
        display+= std::to_string(task->getId()) + " ";
    }
    findHost()->getDisplayString().setTagArg("t", 0, display.c_str());
}

Heartbeat * VirtualServer::generateHeartbeat() {
    Heartbeat* heartbeat = new Heartbeat();
    populateWSM(heartbeat);
    heartbeat->setVirtualServerId(id);
    heartbeat->setComputationPower(computationPower);
    return heartbeat;

}
TaskCompletion * VirtualServer::generateTaskCompletion(double computationWork,int taskId) {
    TaskCompletion* taskCompletion = new TaskCompletion();
    populateWSM(taskCompletion);
    taskCompletion->setVirtualServerId(id);
    taskCompletion->setComputationWork(computationWork);
    taskCompletion->setComputationPower(computationPower);
    taskCompletion->setTaskId(taskId);
    EV_WARN << "Virtual Server with id: " << id
                   << " sending task completion message for a task load of : "
                   << computationWork <<" for the task id of: "<<
                   taskId<< endl;
    return taskCompletion;
}

void VirtualServer::finish(){
    EV_WARN << "Virtual Server with id: " << id<< " time failed "<<timeFailed << endl;
    std::string scTimeFailed = "Micro-cloud :"+ std::to_string(id)+ " failed";
    recordScalar(scTimeFailed.c_str(),timeFailed);
}


