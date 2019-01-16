#ifndef VirtualServer_H
#define VirtualServer_H

#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"
#include "RandomForestMessages/Heartbeat_m.h"
#include "RandomForestMessages/TaskCompletion_m.h"
#include "RandomForestMessages/TaskRequest_m.h"
#include "RandomForestMessages/ConnectionRequest_m.h"
#include "RandomForestMessages/ConnectionApproval_m.h"
#include "RandomForestMessages/ConnectionConfirmation_m.h"



class VirtualServer : public BaseWaveApplLayer {
    public:
        virtual void initialize(int stage);
	protected:
		virtual void onWSM(WaveShortMessage* wsm);
        virtual void handleSelfMsg(cMessage* msg);
	private:
        Heartbeat* generateHeartbeat();
        TaskCompletion* generateTaskCompletion(double computationTask);
        ConnectionRequest* generateConnectionRequest();
        ConnectionConfirmation * generateConnectionConfirmation(int loadBalancerId);
		int computationPower;
		int reliability;
		int id;
        double currentComputationTask;
        double progress;
        int loadBalancerId;
        cMessage* sendWSMEvt;

};

#endif
