#ifndef VirtualServer_H
#define VirtualServer_H

#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"
#include "RandomForestMessages/Heartbeat_m.h"
#include "RandomForestMessages/TaskCompletion_m.h"
#include "RandomForestMessages/TaskRequest_m.h"



class VirtualServer : public BaseWaveApplLayer {
    public:
        virtual void initialize(int stage);
        virtual void finish();
	protected:
		virtual void onWSM(WaveShortMessage* wsm);
        virtual void handleSelfMsg(cMessage* msg);
	private:
        Heartbeat* generateHeartbeat();
        TaskCompletion* generateTaskCompletion(double computationTask,int taskCounter);
        double normalReliability(double expectedReliability);
        bool failed();
        void perhapsFinish();
		double computationPower;
		double reliability;
        double currentComputationWork;
        double progress;
        double latestWorkTime;
        double penaltyTime;
        double timeFailed;
        double penaltyInterval;
        int id;
        int loadBalancerId;
        int taskCounter;
        bool finishCalled;
        bool hasTask;
        cMessage* sendWSMEvt;

};

#endif
