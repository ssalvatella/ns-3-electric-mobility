#include <iostream>
#include <fstream>
#include <sstream>

#include "ns3/log.h"
#include "ns3/node-list.h"
#include "ns3/node.h"
#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/mobility-module.h"
#include "ns3/ns2-mobility-helper.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("MobilityAntwerp");

int 
main (int argc, char *argv[])
{

  NS_LOG_UNCOND ("Mobility Antwerp");

  std::string traceFile;
  std::string vehicleAttributesFile;

  int    nodeNum;
  double duration;
  double updateTime;

  // Parse command line attribute
  CommandLine cmd;
  cmd.AddValue ("traceFile", "Ns2 movement trace file", traceFile);
  cmd.AddValue ("vehicleAttributes", "Vehicle Attributes", vehicleAttributesFile);
  cmd.AddValue ("nodeNum", "Number of nodes", nodeNum);
  cmd.AddValue ("duration", "Duration of Simulation", duration);
  cmd.AddValue ("updateTime", "Time between each update of electric vehicle consumption.", updateTime);
  cmd.Parse (argc,argv);

  // Check command line arguments
  if (traceFile.empty () || vehicleAttributesFile.empty () || nodeNum <= 0 || duration <= 0)
    {
        std::cout << "Usage of " << argv[0] << " :\n\n"
        "./waf --run \"electric-mobility"
        " --traceFile=src/mobility/examples/default.ns_movements"
        " --vehicleAttributes=src/mobility/examples/vehicleAttributes.xml"
        " --nodeNum=2 --duration=100.0\" \n\n"
        "NOTE: ns2-traces-file could be an absolute or relative path. You could use the file default.ns_movements\n"
        "      included in the same directory of this example file.\n\n"
        "NOTE 2: Number of nodes present in the trace file must match with the command line argument and must\n"
        "        be a positive number. Note that you must know it before to be able to load it.\n\n"
        "NOTE 3: Duration must be a positive number. Note that you must know it before to be able to load it.\n\n";

        return 0;
    }

    Ns2MobilityHelper ns2 = Ns2MobilityHelper (traceFile);

    // Create all nodes.
    NodeContainer stas;
    stas.Create (nodeNum);

    NS_LOG_UNCOND ("Instalando trazas ns2...");
    ns2.Install (); // configure movements for each node, while reading trace file

    NS_LOG_UNCOND ("Trazas y modelos eléctricos instalados.");

    //Config::Connect ("/NodeList/*/$ns3::ElectricVehicleConsumptionModel/RemainingEnergy",
    //            MakeCallback (&RemainingEnergyTrace));

    // Log a header for data

    Simulator::Stop (Seconds (duration));

    Simulator::Run ();

    Simulator::Destroy ();

}
