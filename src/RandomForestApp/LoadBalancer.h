#ifndef LoadBalancer_H
#define LoadBalancer_H

#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"
#include "RandomForestMessages/TaskCompletion_m.h"
#include "RandomForestMessages/Heartbeat_m.h"
#include "RandomForestMessages/TaskRequest_m.h"
#include "Microcloud.h"
#include <omnetpp.h>

class LoadBalancer : public BaseWaveApplLayer {
    public:
        virtual void initialize(int stage);
        virtual void finish();
	protected:
		virtual void onWSM(WaveShortMessage* wsm);
        virtual void handleSelfMsg(cMessage* msg);
	private:
        void distributeWorkLoad(double computationTask);
        double clearFailingMicroClouds();
        bool isAnMCIdle();
        TaskRequest* generateTaskRequest(double virtualServertask,int virtualServerId,int taskCounter);
        void perhapsFinish();
        void sendFirstTask();

        std::map<int,MicroCloud *> idToMicrocloud;
        cMessage* sendWSMEvt;

        double heartBeatTimeout;
        double currentComputationWork;
        double failedWork;
        double workFinished;
        double progress;
        double expectedComputationWork;
        double replicationFactor;

        int id;
        int upperBound;
        int lowerBound;
        int taskId;

        bool reputationOn;
        bool sentFirstTask;
        bool finishCalled;

        cOutVector taskCompletedVector;
        cOutVector workFinishedVector;
};

#endif
