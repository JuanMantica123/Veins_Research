package RandomForestApp;
import org.car2x.veins.modules.application.ieee80211p.BaseWaveApplLayer;

simple LoadBalancer extends BaseWaveApplLayer

{       
    parameters:
        double expectedComputationTask;
        double replicationFactor = default(1);
        int id;
        int lowerBound;
        int upperBound;
        int timeOutIntervals;
        bool reputationOn = default(false);
        
        
    @class(LoadBalancer);
    @display("i=block/app2");
}
