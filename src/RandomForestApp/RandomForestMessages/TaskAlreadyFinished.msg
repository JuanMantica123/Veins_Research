cplusplus {{
#include "veins/modules/messages/WaveShortMessage_m.h"
}}

class WaveShortMessage;

packet TaskAlreadyFinished extends WaveShortMessage {
	int loadBalancerId;
	int taskId;
}
