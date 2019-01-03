#include "CNNAppRSU.h"

#ifndef DBG_APP
#define EV_WARN
#endif

Define_Module(CNNAppRSU);


void CNNAppRSU::initialize(int stage){
    BaseWaveApplLayer::initialize(stage);
    if(stage==0){
        storedValues = {1,2,3,4};
    } else if (stage == 1) {
//        std::string currentRoadOnRoute = traciVehicle->getCurrentRoadOnRoute();
//        std::string roadId = traciVehicle->getRoadId();
//        EV_WARN<<<<endl;
//        EV_WARN<<traciVehicle->getRoadId()<<endl;
    }

}

void CNNAppRSU::onWSM(WaveShortMessage* wsm) {
    if (RequestMessage* request = dynamic_cast<RequestMessage*>(wsm)){
        int requestedValue = request->getRequestedValue();
        EV_WARN<<"Requested value = " << requestedValue<<endl;
        if(storedValues.find(requestedValue)!=storedValues.end()){
            EV_WARN<<"Sending requested value"<<endl;
            ResponseMessage * response = new ResponseMessage();
            response->setResponseValue(requestedValue);
            populateWSM(response);
            sendDelayedDown(response, uniform(0.01,0.2));
        }
    }
}

