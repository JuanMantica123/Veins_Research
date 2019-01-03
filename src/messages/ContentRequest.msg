cplusplus {{
#include "veins/modules/messages/WaveShortMessage_m.h"
#include "VectorWrapper.h"
}}

class WaveShortMessage;
class noncobject VectorWrapper; 
packet ContentRequest extends WaveShortMessage {
    string carId;
    int currentRoad;
    VectorWrapper plannedRoadIds;
	
	
	
	
}
