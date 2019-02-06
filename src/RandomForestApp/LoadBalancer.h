#ifndef LoadBalancer_H
#define LoadBalancer_H

#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"
#include "RandomForestMessages/TaskCompletion_m.h"
#include "RandomForestMessages/Heartbeat_m.h"
#include "RandomForestMessages/TaskRequest_m.h"
#include "Microcloud.h"

class LoadBalancer : public BaseWaveApplLayer {
    public:
        virtual void initialize(int stage);
	protected:
		virtual void onWSM(WaveShortMessage* wsm);
        virtual void handleSelfMsg(cMessage* msg);
	private:
        void distributeWorkLoad(double computationTask);
        double clearFailingMicroClouds();
        TaskRequest* generateTaskRequest(double virtualServertask,int virtualServerId);
        std::map<int,MicroCloud> idToMicrocloud;
        cMessage* sendWSMEvt;
        int id;
        int upperBound;
        int lowerBound;
        double heartBeatTimeout;
        double currentComputationTask;
};

#endif
