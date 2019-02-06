package RandomForestApp;
import org.car2x.veins.modules.application.ieee80211p.BaseWaveApplLayer;

simple LoadBalancer extends BaseWaveApplLayer

{       
    parameters:
        int id;
        int lowerBound;
        int upperBound;
        
        
    @class(LoadBalancer);
    @display("i=block/app2");
}
