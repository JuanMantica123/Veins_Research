cplusplus {{
#include "veins/modules/messages/WaveShortMessage_m.h"
}}

class WaveShortMessage;

packet ConnectionRequest extends WaveShortMessage {
	int virtualServerId;
}