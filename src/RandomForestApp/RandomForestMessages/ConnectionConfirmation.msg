cplusplus {{
#include "veins/modules/messages/WaveShortMessage_m.h"
}}

class WaveShortMessage;

packet ConnectionConfirmation extends WaveShortMessage {
	int virtualServerId;
	int loadBalancerId;
	int computationPower;
}