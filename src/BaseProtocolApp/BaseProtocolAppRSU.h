#ifndef BaseProtocolAppRSU_H
#define BaseProtocolAppRSU_H

#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"
#include <unordered_set>

class BaseProtocolAppRSU : public BaseWaveApplLayer {
    public:
        virtual void initialize(int stage);
	protected:
		virtual void onWSM(WaveShortMessage* wsm);
	private:
		std::unordered_set<int> storedValues;
		std::string rsuSignature;
		std::string carSignature;
};

#endif
