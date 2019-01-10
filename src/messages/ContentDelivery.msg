cplusplus {{
#include "veins/modules/messages/WaveShortMessage_m.h"
}}

class WaveShortMessage;

packet ContentDelivery extends WaveShortMessage {
	string carId;
	int rsuId;
}
