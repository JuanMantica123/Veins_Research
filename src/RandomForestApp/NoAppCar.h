#ifndef NoAppCar_H
#define NoAppCar_H

#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"

class NoAppCar : public BaseWaveApplLayer {
    public:
        virtual void initialize(int stage);
    private:
        void printCoord(double x, double y);
};

#endif
