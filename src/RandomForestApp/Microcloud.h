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

    int getLatestComputationTask() const {
        return latestComputationTask;
    }

    void setLatestComputationTask(int latestComputationTask) {
        this->latestComputationTask = latestComputationTask;
    }

    bool isFunctioning() const {
        return functioning;
    }

    void setFunctioning(bool functioning) {
        this->functioning = functioning;
    }

private:
    int virtualServerId;
    double lastHeartbeat;
    bool idle;
    bool functioning;
    int computationPower;
    int latestComputationTask;
    int reputation;

};

#endif
