cplusplus {{
#include "veins/modules/messages/WaveShortMessage_m.h"
}}

class WaveShortMessage;

packet ConnectionApproval extends WaveShortMessage {
	int virtualServerId;
	int loadBalancerId;
}