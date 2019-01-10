package CNNApp;
import org.car2x.veins.modules.application.ieee80211p.BaseWaveApplLayer;

simple CNNAppRSU extends BaseWaveApplLayer

{
    parameters:
        int rsuId = default(-1);
        
    @class(CNNAppRSU);
    @display("i=block/app2");
    
}
