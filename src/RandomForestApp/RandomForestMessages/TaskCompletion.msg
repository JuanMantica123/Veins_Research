cplusplus {{
#include "veins/modules/messages/WaveShortMessage_m.h"
}}

class WaveShortMessage;

packet TaskCompletion extends WaveShortMessage {
	int virtualServerId;
	int taskId;
	double computationWork;
	double computationPower;
}
