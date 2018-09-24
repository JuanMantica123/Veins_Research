#ifndef BaseProtocolAppCar_H
#define BaseProtocolAppCar_H

#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"

class BaseProtocolAppCar : public BaseWaveApplLayer {
    public:
        virtual void initialize(int stage);
    protected:
        virtual void onWSM(WaveShortMessage* wsm);
        virtual void handleSelfMsg(cMessage* msg);
    private:
        WaveShortMessage* generateValuePetitionWSM();
        simtime_t interval;
        cMessage* sendWSMEvt;
        int lookedValue;
        std::string rsuSignature;
        std::string carSignature;
        bool found;
};

#endif
