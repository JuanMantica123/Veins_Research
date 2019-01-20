#ifndef LoadBalancer_H
#define LoadBalancer_H

#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"
#include "RandomForestMessages/TaskCompletion_m.h"
#include "RandomForestMessages/ConnectionRequest_m.h"
#include "RandomForestMessages/ConnectionApproval_m.h"
#include "RandomForestMessages/ConnectionConfirmation_m.h"
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
        void distributeWorkLoad();
        void clearFailingMicroClouds();
        TaskRequest* generateTaskRequest(double virtualServertask,int virtualServerId);
        ConnectionApproval * generateConnectionApproval(int virtualServerId);
        int id;
        int computationPower;
        int heartbeatTime;
        double currentComputationTask;
	    std::map<int,MicroCloud> idToMicrocloud;
        cMessage* sendWSMEvt;
};

#endif
