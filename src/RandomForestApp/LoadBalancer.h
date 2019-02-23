#ifndef LoadBalancer_H
#define LoadBalancer_H

#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"
#include "RandomForestMessages/TaskCompletion_m.h"
#include "RandomForestMessages/Heartbeat_m.h"
#include "RandomForestMessages/TaskRequest_m.h"
#include "RandomForestMessages/TaskAlreadyFinished_m.h"
#include "Microcloud.h"
#include <omnetpp.h>
#include <unordered_set>

class LoadBalancer : public BaseWaveApplLayer {
    public:
        virtual void initialize(int stage);
        virtual void finish();
	protected:
		virtual void onWSM(WaveShortMessage* wsm);
        virtual void handleSelfMsg(cMessage* msg);
	private:
        struct MC_Compare_Work {
             bool operator() (MicroCloud * a, MicroCloud * b) const {
                 return b->getCurrentWorkLoad()>a->getCurrentWorkLoad();
             }
         };

        void createNewTask(int taskId);
        void sendTasks();
        int sendTask(std::set<MicroCloud *,MC_Compare_Work> functioningClouds);
        void clearFailingMcs();
        void perhapsAddFailedTask(std::deque<Task *> failedTasks, int mcId);
        void deleteTaskFromMCs(int taskId);

        TaskRequest* generateTaskRequest(double virtualServerWork,int virtualServerId,int taskId);
        TaskAlreadyFinished * generateTaskAlreadyFinished(int taskId);

        struct Task_Compare {
            bool operator() (Task * a, Task * b) const {
                return b->getId()>a->getId();
            }
        };


        std::set<Task *, Task_Compare> tasks;


        std::map<int,MicroCloud *> idToMicrocloud;
        std::map<int,std::set<MicroCloud *>> taskIdToMcs;


        std::unordered_set<int> finishedTaskIds;

        double heartBeatTimeout;
        double failedWork;
        double workFinished;
        double expectedComputationWork;
        double replicationFactor;

        int id;
        int upperBound;
        int lowerBound;
        int desiredNumTask;

        bool reputationOn;


        cOutVector workFinishedVector;
        cMessage* sendWSMEvt;
};

#endif
