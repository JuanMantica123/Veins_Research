cplusplus {{
#include "veins/modules/messages/WaveShortMessage_m.h"
}}

class WaveShortMessage;

packet ContentResponse extends WaveShortMessage {
	string carId;
	int rsuId;
}
