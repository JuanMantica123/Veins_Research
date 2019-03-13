#ifndef VirtualServer_H
#define VirtualServer_H


#include <deque>
#include "Task.h"
#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"
#include "RandomForestMessages/Heartbeat_m.h"
#include "RandomForestMessages/TaskCompletion_m.h"
#include "RandomForestMessages/TaskRequest_m.h"
#include "RandomForestMessages/TaskAlreadyFinished_m.h"



class VirtualServer : public BaseWaveApplLayer {
    public:
        virtual void initialize(int stage);
        virtual void finish();
        virtual void refreshDisplay() const;
	protected:
		virtual void onWSM(WaveShortMessage* wsm);
        virtual void handleSelfMsg(cMessage* msg);
	private:
        Heartbeat* generateHeartbeat();
        TaskCompletion* generateTaskCompletion(double computationWork,int taskId);
        double normalReliability(double expectedReliability);
        void deleteTask(int id);
        bool failed(int time);
        void fail();
        void loadNewTask(double unacountedProgress);

        std::deque<Task *> tasks;

        std::map<int,double> timeToComputationPower;
        std::map<int,int> timetoNumCars;

        Task * currentTask;

		double computationPower;
		double reliability;
        double latestWorkTime;
        double penaltyTime;
        double timeFailed;
        double penaltyInterval;
        double latestTimeFailedCalled;

        int minNumCars;
        int id;
        int loadBalancerId;

        cMessage* sendWSMEvt;

};

#endif
