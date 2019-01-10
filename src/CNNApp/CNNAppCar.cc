#include "CNNAppCar.h"

#ifndef DBG_APP
#define EV_WARN
#endif

Define_Module(CNNAppCar);

void CNNAppCar::initialize(int stage) {
    BaseWaveApplLayer::initialize(stage);
    if (stage == 0) {
        std::list<std::string> plannedRoadIds = traciVehicle->getPlannedRoadIds();
        std::vector<int> vectorRoadIds;
        for (std::string id : plannedRoadIds) {
            vectorRoadIds.push_back(std::stoi(id));
        }
        roadIds.vector = vectorRoadIds;
        EV_WARN << "Planned road ids: "<<endl;
        for (int roadId : roadIds.vector){
            EV_WARN<< roadId<< " ";
        }
        EV_WARN<<endl;
        carId = mobility->getExternalId();
        sendWSMEvt = new cMessage("wsm evt");
        for (int id = 0; id <= 3; ++id){
            idValuesNeeded.insert(id);
        }
        allValuesFound = false;
    } else if (stage == 1) {
        scheduleAt(simTime() + beaconInterval, sendWSMEvt);
    }
}

void CNNAppCar::handleSelfMsg(cMessage* msg) {
    if (!allValuesFound) {
        std::string strRoadId = traciVehicle->getRoadId();
        try{
            int roadId = std::stoi(strRoadId);
            if (foundIdValues.find(roadId) == foundIdValues.end()){
                EV_WARN << "Requesting from RSU with road id: " << roadId << endl;
                sendDown(generateCDMessage(roadId));
            }
        }
        catch(std::invalid_argument e){
            EV_WARN << "Invalid road id" << strRoadId << endl;

        }
        scheduleAt(simTime() + beaconInterval + uniform(0.5, 1), msg);
    }
}

void CNNAppCar::onWSM(WaveShortMessage* wsm) {
    if (!allValuesFound) {
        if (ContentDelivery* response = dynamic_cast<ContentDelivery*>(wsm)) {
            if (strcmp(response->getCarId(), carId.c_str()) == 0) {
                int rsuId = response->getRsuId();
                foundIdValues.insert(rsuId);
                if (foundIdValues == idValuesNeeded) {
                    EV_WARN << "Found all values needed"<<endl;
                    allValuesFound = true;
                }
            }
        }
    }
}

ContentRequest* CNNAppCar::generateCDMessage(int roadId) {
    ContentRequest* contentRequest = new ContentRequest();
    contentRequest->setCurrentRoad(roadId);
    contentRequest->setPlannedRoadIds(roadIds);
    populateWSM(contentRequest);
    contentRequest->setCarId(carId.c_str());
    return contentRequest;
}

