cplusplus {{
#include "veins/modules/messages/WaveShortMessage_m.h"
}}

class WaveShortMessage;

packet Heartbeat extends WaveShortMessage {
	int virtualServerId;
	double computationPower;
}
