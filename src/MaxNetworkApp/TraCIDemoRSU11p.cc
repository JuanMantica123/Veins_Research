#include "TraCIDemoRSU11p.h"

#ifndef DBG_APP
#define EV_WARN
#endif

Define_Module(TraCIDemoRSU11p);

void TraCIDemoRSU11p::onWSM(WaveShortMessage* wsm) {
    //this rsu repeats the received traffic update in 2 seconds plus some random delay
    EV_WARN<<"RSu received message at = "<<simTime()<<endl;
    sendDelayedDown(wsm->dup(), 2 + uniform(0.01,0.2));

}
