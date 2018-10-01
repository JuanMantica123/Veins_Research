cplusplus {{
#include "veins/modules/messages/WaveShortMessage_m.h"
}}

class WaveShortMessage;

packet RequestMessage extends WaveShortMessage {
	int requestedValue;
}
