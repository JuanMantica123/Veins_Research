cplusplus {{
#include "veins/modules/messages/WaveShortMessage_m.h"
}}

class WaveShortMessage;

packet TaskRequest extends WaveShortMessage {
   	int taskId;
	int virtualServerId;
	int loadBalancerId;
	double computationWork;
}
