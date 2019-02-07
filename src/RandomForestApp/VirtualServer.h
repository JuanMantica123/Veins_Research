#ifndef VirtualServer_H
#define VirtualServer_H

#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"
#include "RandomForestMessages/Heartbeat_m.h"
#include "RandomForestMessages/TaskCompletion_m.h"
#include "RandomForestMessages/TaskRequest_m.h"



class VirtualServer : public BaseWaveApplLayer {
    public:
        virtual void initialize(int stage);
	protected:
		virtual void onWSM(WaveShortMessage* wsm);
        virtual void handleSelfMsg(cMessage* msg);
	private:
        Heartbeat* generateHeartbeat();
        TaskCompletion* generateTaskCompletion(double computationTask,int taskCounter);
        double normalReliability();
		double computationPower;
		double reliability;
        double currentComputationTask;
        double progress;
        double latestWorkTime;
        double penaltyTime;
        int id;
        int loadBalancerId;
        int taskCounter;
        cMessage* sendWSMEvt;

};

#endif
