#include "TraCIDemo11p.h"
#include <stdlib.h>

#ifndef DBG_APP
#define EV_WARN
#endif

Define_Module(TraCIDemo11p);


void TraCIDemo11p::initialize(int stage) {
    BaseWaveApplLayer::initialize(stage);
    if (stage == 0) {
        max = 0;
        interval = 1;
        sendWSMEvt = new cMessage("wsm evt");
    }
    else if(stage==1){
        scheduleAt(simTime()+interval, sendWSMEvt);
    }
}
void TraCIDemo11p::handleSelfMsg(cMessage* msg){
    sendDown(generateRandomNumWSM());
    scheduleAt(simTime()+interval, msg);
}

void TraCIDemo11p::onWSM(WaveShortMessage* wsm) {
    std::string strMax(wsm->getWsmData());
    float otherCarMax=std::stof(strMax);
    updateMax(otherCarMax);
}

WaveShortMessage* TraCIDemo11p::generateRandomNumWSM(){
    float randomSentNumber = (rand() % 100) + 1.0;
    std::string message = std::to_string(randomSentNumber);
    WaveShortMessage* wsm = new WaveShortMessage();
    populateWSM(wsm);
    wsm->setWsmData(message.c_str());
    return wsm;
}



void TraCIDemo11p::updateMax(float newRandomNumber){
    if(newRandomNumber > max){
        max = newRandomNumber;
        EV_WARN << "***New_Max = "<< max << "***" << endl;
    }

}
