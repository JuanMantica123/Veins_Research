package RandomForestApp;
import org.car2x.veins.modules.application.ieee80211p.BaseWaveApplLayer;

simple VirtualServer extends BaseWaveApplLayer

{
    parameters:
        int computationPower;
        int reliability;
        
    @class(VirtualServer);
    @display("i=block/app2");
    
}
