//
// Copyright (C) 2006-2011 Christoph Sommer <christoph.sommer@uibk.ac.at>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "veins/modules/application/traci/TraCIDemo11p.h"
#include <stdlib.h>
#ifndef DBG_APP
#define DBG_APP EV_WARN
#endif

Define_Module(TraCIDemo11p);


void TraCIDemo11p::initialize(int stage) {
    BaseWaveApplLayer::initialize(stage);
    if (stage == 0) {
        max = 0;//Here I generate random number
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
//    EV_WARN << "***Max from other car = *"<< strMax<<  endl;
//    EV_WARN << "***Max from other car = *"<< otherCarMax<<  endl;
    updateMax(otherCarMax);
}

WaveShortMessage* TraCIDemo11p::generateRandomNumWSM(){
    float randomSentNumber = (rand() % 100) + 1.0;
    std::string message = std::to_string(randomSentNumber);
    //instead of a generating new random I would just send the current
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
