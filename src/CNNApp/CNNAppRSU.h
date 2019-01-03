#ifndef CNNAppRSU_H
#define CNNAppRSU_H

#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"
#include <unordered_set>
#include "../messages/RequestMessage_m.h"
#include "../messages/ResponseMessage_m.h"

class CNNAppRSU : public BaseWaveApplLayer {
    public:
        virtual void initialize(int stage);
	protected:
		virtual void onWSM(WaveShortMessage* wsm);
	private:
		std::unordered_set<int> storedValues;
};

#endif
