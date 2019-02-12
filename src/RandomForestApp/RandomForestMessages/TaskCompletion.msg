cplusplus {{
#include "veins/modules/messages/WaveShortMessage_m.h"
}}

class WaveShortMessage;

packet TaskCompletion extends WaveShortMessage {
	int virtualServerId;
	int taskCounter;
	double computationWork;
	double computationPower;
}
