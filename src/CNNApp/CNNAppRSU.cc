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
        rsuId = par("rsuId").longValue();
        EV_WARN<<"Initiated with rsu id:  "<<rsuId<<endl;

    }

}

void CNNAppRSU::onWSM(WaveShortMessage* wsm) {
    if (ContentRequest* request = dynamic_cast<ContentRequest*>(wsm)){
        int roadId = request->getCurrentRoad();
        EV_WARN<<"Getting a requested value with the road id of:  "<<roadId<<" the rsu id is : "<<rsuId<<endl;
        if(roadId==rsuId){
            EV_WARN<<"Road id matches rsu id sending requested value back"<<endl;
            ContentDelivery* response = new ContentDelivery();
            response->setCarId(request->getCarId());
            response->setRsuId(rsuId);
            populateWSM(response);
            sendDelayedDown(response, uniform(0.01,0.2));
        }
    }
}

