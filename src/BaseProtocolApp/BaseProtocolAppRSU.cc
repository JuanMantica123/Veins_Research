#include "BaseProtocolAppRSU.h"
#include <string>
#include <sstream>
#include <vector>
#include "SplitHelper.h"

#ifndef DBG_APP
#define EV_WARN
#endif

Define_Module(BaseProtocolAppRSU);


void BaseProtocolAppRSU::initialize(int stage){
    BaseWaveApplLayer::initialize(stage);
    if(stage==0){
        storedValues = {1,2,3,4};

        //I am not sure how to place this enums into a file?
        rsuSignature = "1";
        carSignature = "2";
    }
}

void BaseProtocolAppRSU::onWSM(WaveShortMessage* wsm) {

    //Here I am attempting to overgo the limitation that wsmData is sent as a string

    std::string wsmMessage(wsm->getWsmData());
    //In this message tuple the first value indicates the signature of the entity that sent the message
    // the second value indicates the value that is looked for
    std::vector<std::string> messageTuple = split(wsmMessage, ',');
    std::string signature = messageTuple.at(0);
    int lookedValue = stoi(messageTuple.at(1));

    EV_WARN<<"Signature ="<<signature << "looked for value = " << lookedValue<<endl;
    if(signature==carSignature){
        if(storedValues.find(lookedValue)!=storedValues.end()){
            EV_WARN<<"Sending wanted value"<<endl;
            WaveShortMessage * wsm  = new WaveShortMessage();
            populateWSM(wsm);
            std::string lookedValueStr = std::to_string(lookedValue);
            std::string dataTuple = rsuSignature +',' + lookedValueStr;
            wsm->setWsmData(dataTuple.c_str());
            sendDelayedDown(wsm, uniform(0.01,0.2));

        }
    }
}

