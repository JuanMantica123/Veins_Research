package RandomForestApp;
import org.car2x.veins.modules.application.ieee80211p.BaseWaveApplLayer;

simple VirtualServer extends BaseWaveApplLayer

{
    parameters:
        double penaltyInterval;
        double expectedReliability;
        double computationPower;
        int id;
        int loadBalancerId;
        int minNumCars = default(3);
        
    @class(VirtualServer);
    @display("i=block/app2");
    
}
