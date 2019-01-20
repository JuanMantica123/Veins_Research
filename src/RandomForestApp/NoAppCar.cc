#include "NoAppCar.h"

#ifndef DBG_APP
#define EV_WARN
#endif

Define_Module(NoAppCar);


void NoAppCar::initialize(int stage){
    BaseWaveApplLayer::initialize(stage);


}
void NoAppCar::printCoord(double x, double y){
    Veins::TraCICoord c = {x,y};
    Coord omnetCoord = traciVehicle->traci2Omnet(c);
    std::cout<<omnetCoord<<endl;
}
