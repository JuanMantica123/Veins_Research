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

    bool isIdle() const {
        return idle;
    }

    void setIdle(bool idle) {
        this->idle = idle;
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
    void setReputation(double reputation) {
        this->reputation = reputation;
    }

    double getLatestComputationWork() const {
        return latestComputationWork;
    }

    void setLatestComputationWork(double latestComputationWork) {
        this->latestComputationWork = latestComputationWork;
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

    void setWorkDone(double workDone) {
        this->workDone = workDone;
    }

    bool isLatestTaskReplicated() const {
        return latestTaskReplicated;
    }

    void setLatestTaskReplicated(bool latestTaskReplicated) {
        this->latestTaskReplicated = latestTaskReplicated;
    }

    int getTaskCounter() const {
        return taskCounter;
    }

    void initializeCounter(){
        this->taskCounter = 0;
    }
    void incrementCounter() {
        this->taskCounter++;
    }

    int getTaskId() const {
        return taskId;
    }

    void setTaskId(int taskId) {
        this->taskId = taskId;
    }

private:
    double latestComputationWork;
    double lastHeartbeat;
    double reputation;
    double workDone;
    int virtualServerId;
    int computationPower;
    int taskCounter;
    int taskId;
    bool idle;
    bool functioning;
    bool latestTaskReplicated;

};

#endif
