cplusplus {{
#include "veins/modules/messages/WaveShortMessage_m.h"
}}

class WaveShortMessage;

packet ResponseMessage extends WaveShortMessage {
	int responseValue;
}
