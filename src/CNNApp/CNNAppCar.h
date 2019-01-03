#ifndef CNNAppCar_H
#define CNNAppCar_H

#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"
#include "../messages/ContentRequest_m.h"
#include "../messages/ContentResponse_m.h"

class CNNAppCar : public BaseWaveApplLayer {
    public:
        virtual void initialize(int stage);
    protected:
        virtual void onWSM(WaveShortMessage* wsm);
        virtual void handleSelfMsg(cMessage* msg);
    private:
        VectorWrapper roadIds;
        std::string carId;
        std::set<int> foundIdValues;
        std::set<int> idValuesNeeded;
        ContentRequest* generateCDMessage(int roadId);
        cMessage* sendWSMEvt;
        bool allValuesFound;
};

#endif
