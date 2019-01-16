#include "NoAppCar.h"

#ifndef DBG_APP
#define EV_WARN
#endif

Define_Module(NoAppCar);


void NoAppCar::initialize(int stage){
    BaseWaveApplLayer::initialize(stage);

}
