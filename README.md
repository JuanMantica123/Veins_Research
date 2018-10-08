##Veins Applications for Research on VANETS By Juan Mantica and Lewis Tseng
Veins is a simulation library built on the network simulation framework that is Omnet. Veins couple Omnet with the Simulation of Urban Mobility Framework,or SUMO, in order to create a framework for the simulation of vehicular networks.
To understand more about what is VEINS, SUMO and Omnett, please refer to the following links: https://veins.car2x.org/, https://omnetpp.org/doc/omnetpp/manual/, and http://sumo.dlr.de/index.html.

For our research we will be creating applications on VEINS to model real world vehicular networks, that is through the use of VEINS we will be able to realistically understand how the movement of cars affect the vehicular network and how does the network affect the movements of cars.

##Technical Details
As explained earlier VEINS is the glue that holds Omnet together with SUMO. To create a VEINS simulation we first create a scenario in SUMO, this scenario can be an intersection, a highway or an entire city. Regardless of the scenario Veins will take care of making nodes in omnett out of the cars that enter this scenario, and it will also take care of updating the position of these nodes as the cars move through the scenario.

Once the SUMO scenario is finished, the application for the cars must be created in Omnett. Note that Omnet is an event driven framework, so the application actions of a node will be determined by A) the first event in initialization and B) all subsequent events that affect that node.


