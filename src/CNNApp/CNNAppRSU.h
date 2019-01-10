#ifndef CNNAppRSU_H
#define CNNAppRSU_H

#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"
#include <unordered_set>
#include "../messages/ContentRequest_m.h"
#include "../messages/ContentDelivery_m.h"

class CNNAppRSU : public BaseWaveApplLayer {
    public:
        virtual void initialize(int stage);
	protected:
		virtual void onWSM(WaveShortMessage* wsm);
	private:
		std::unordered_set<int> storedValues;
		int rsuId;
};

#endif
