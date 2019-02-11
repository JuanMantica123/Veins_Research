package RandomForestApp;
import org.car2x.veins.modules.application.ieee80211p.BaseWaveApplLayer;

simple VirtualServer extends BaseWaveApplLayer

{
    parameters:
        double penaltyInterval;
        double expectedReliability;
        int computationPower;
        int id;
        int loadBalancerId;
        
    @class(VirtualServer);
    @display("i=block/app2");
    
}
