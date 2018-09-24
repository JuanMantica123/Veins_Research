#include "BaseProtocolAppCar.h"
#include <stdlib.h>
#include <string>
#include "SplitHelper.h"

#ifndef DBG_APP
#define EV_WARN
#endif

Define_Module(BaseProtocolAppCar);


void BaseProtocolAppCar::initialize(int stage) {
    BaseWaveApplLayer::initialize(stage);
    if (stage == 0) {
        interval = 1;
        lookedValue = (rand() % 4) + 1.0;
        found = false;
        sendWSMEvt = new cMessage("wsm evt");
        rsuSignature = "1";
        carSignature = "2";
    }
    else if(stage==1){
        scheduleAt(simTime()+interval+uniform(0.01,0.2), sendWSMEvt);
    }
}

void BaseProtocolAppCar::handleSelfMsg(cMessage* msg){
    if(!found){
        EV_WARN<<"Asking for value of "<<lookedValue<<endl;
        sendDown(generateValuePetitionWSM());
        scheduleAt(simTime()+interval+uniform(0.01,0.2), msg);
    }
}

void BaseProtocolAppCar::onWSM(WaveShortMessage* wsm) {
    if(!found){
        std::string wsmMessage(wsm->getWsmData());
        std::vector<std::string> messageTuple = split(wsmMessage, ',');
        std::string signature = messageTuple.at(0);
        int valueFromOther = stoi(messageTuple.at(1));
        EV_WARN<<"Received signature :"<<signature<<"Received value :"<<valueFromOther<<endl;
        if(signature==rsuSignature && valueFromOther==lookedValue){
            EV_WARN<<"Wanted value was found"<<endl;
            found = true;
        }
    }

}

WaveShortMessage* BaseProtocolAppCar::generateValuePetitionWSM(){
    WaveShortMessage* wsm = new WaveShortMessage();
    populateWSM(wsm);
    std::string lookedValueStr =std::to_string(lookedValue);
    std::string messageTuple = carSignature +','+lookedValueStr;
    wsm->setWsmData(messageTuple.c_str());
    return wsm;
}


