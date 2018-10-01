#include "BaseProtocolAppCar.h"

#ifndef DBG_APP
#define EV_WARN
#endif

Define_Module(BaseProtocolAppCar);


void BaseProtocolAppCar::initialize(int stage) {
    BaseWaveApplLayer::initialize(stage);
    if (stage == 0) {
        requestedValue = (rand() % 4) + 1.0;
        found = false;
    }
}

void BaseProtocolAppCar::handleSelfMsg(cMessage* msg){
    if(!found){
        EV_WARN<<"Asking for value of "<<requestedValue<<endl;
        sendDown(generateRequestMessage());
        scheduleAt(simTime()+beaconInterval+uniform(0.01,0.2), msg);
    }
}

void BaseProtocolAppCar::onWSM(WaveShortMessage* wsm) {
    if (!found){
        if (ResponseMessage* response = dynamic_cast<ResponseMessage*>(wsm)) {
            int responseValue = response->getResponseValue();
            if(responseValue == requestedValue)
                found = true;
        }
    }
}

RequestMessage* BaseProtocolAppCar::generateRequestMessage(){
    RequestMessage* requestMessage = new RequestMessage();
    populateWSM(requestMessage);
    requestMessage->setRequestedValue(requestedValue);
    return requestMessage;
}


