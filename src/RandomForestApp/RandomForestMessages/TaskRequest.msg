cplusplus {{
#include "veins/modules/messages/WaveShortMessage_m.h"
}}

class WaveShortMessage;

packet TaskRequest extends WaveShortMessage {
	int virtualServerId;
	int loadBalancerId;
	int taskCounter;
	double computationTask;
}
