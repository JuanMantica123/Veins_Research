#include "Task.h"
#include <assert.h>
#ifndef MicroCloud_H
#define MicroCloud_H

class MicroCloud {
public:
    int getComputationPower() const {
        return computationPower;
    }

    void setComputationPower(int computationPower) {
        this->computationPower = computationPower;
    }

    double getLastHeartbeat() const {
        return lastHeartbeat;
    }

    void setLastHeartbeat(double lastHeartbeat) {
        this->lastHeartbeat = lastHeartbeat;
    }

    int getVirtualServerId() const {
        return virtualServerId;
    }

    void setVirtualServerId(int virtualServerId) {
        this->virtualServerId = virtualServerId;
    }

    double getReputation() const {
        return reputation;
    }
    void changeReputation(double change){
        this->reputation += change;
    }
    void setReputation(double reputation){
        this->reputation = reputation;
    }
    bool isFunctioning() const {
        return functioning;
    }

    void setFunctioning(bool functioning) {
        this->functioning = functioning;
    }

    double getWorkDone() const {
        return workDone;
    }
    void setWorkDone(double workDone){
        this->workDone = workDone;
    }

    void incrementWorkDone(double latestWorkDone) {
        this->workDone = latestWorkDone;
    }
    void pushTask(Task * task){
        tasks.push_back(task);
        currentWorkLoad+=task->getWorkLoad();

    }
    int getLatestTaskId(){
        return tasks.front()->getId();
    }

    void recordFailure(){
        tasks.clear();
        currentWorkLoad = 0;
        functioning =false;
    }

    double getCurrentWorkLoad() const {
        return currentWorkLoad;
    }

    const std::deque<Task*>& getTasks() const {
        return tasks;
    }
    void deleteTask(int taskId) {
        for (auto task = tasks.begin(); task != tasks.end(); ++task) {
            Task * iteratingTask = *task;
            if (iteratingTask->getId() == taskId) {
                currentWorkLoad -= iteratingTask->getWorkLoad();
                tasks.erase(task);
                break;
            }
        }
    }

private:
    std::deque<Task*> tasks;
    double lastHeartbeat;
    double reputation;
    double workDone;
    double currentWorkLoad;
    int virtualServerId = 1;
    int computationPower;
    bool functioning;

};

#endif
