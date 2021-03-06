#ifndef TraCIDemo11p_H
#define TraCIDemo11p_H

#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"

class TraCIDemo11p : public BaseWaveApplLayer {
    public:
        virtual void initialize(int stage);
    protected:
        virtual void onWSM(WaveShortMessage* wsm);
        virtual void handleSelfMsg(cMessage* msg);
    private:
        void updateMax(float newRandomNumer);
        WaveShortMessage* generateRandomNumWSM();
        float max;
        simtime_t interval;
        cMessage* sendWSMEvt;

};

#endif
