#ifndef TraCIDemoRSU11p_H
#define TraCIDemoRSU11p_H

#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"

class TraCIDemoRSU11p : public BaseWaveApplLayer {
	protected:
		virtual void onWSM(WaveShortMessage* wsm);
};

#endif
