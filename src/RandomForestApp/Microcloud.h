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

    int getReputation() const {
        return reputation;
    }

    void setReputation(int reputation) {
        this->reputation = reputation;
    }

    double getLatestComputationTask() const {
        return latestComputationTask;
    }

    void setLatestComputationTask(double latestComputationTask) {
        this->latestComputationTask = latestComputationTask;
    }

    bool isFunctioning() const {
        return functioning;
    }

    void setFunctioning(bool functioning) {
        this->functioning = functioning;
    }

    int getTaskCounter() const {
        return taskCounter;
    }

    void setTaskCounter(int taskCounter) {
        this->taskCounter = taskCounter;
    }

private:
    double latestComputationTask;
    double lastHeartbeat;
    int virtualServerId;
    int computationPower;
    int reputation;
    int taskCounter;
    bool idle;
    bool functioning;

};

#endif
