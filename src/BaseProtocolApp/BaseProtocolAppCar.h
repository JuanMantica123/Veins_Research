#ifndef BaseProtocolAppCar_H
#define BaseProtocolAppCar_H

#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"
#include "../messages/RequestMessage_m.h"
#include "../messages/ResponseMessage_m.h"

class BaseProtocolAppCar : public BaseWaveApplLayer {
    public:
        virtual void initialize(int stage);
    protected:
        virtual void onWSM(WaveShortMessage* wsm);
        virtual void handleSelfMsg(cMessage* msg);
    private:
        RequestMessage* generateRequestMessage();
        int requestedValue;
        bool found;
};

#endif
