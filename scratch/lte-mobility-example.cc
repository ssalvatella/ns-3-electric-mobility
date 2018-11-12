 #include "ns3/lte-helper.h"
 #include "ns3/epc-helper.h"
 #include "ns3/core-module.h"
 #include "ns3/network-module.h"
 #include "ns3/ipv4-global-routing-helper.h"
 #include "ns3/internet-module.h"
 #include "ns3/mobility-module.h"
 #include "ns3/lte-module.h"
 #include "ns3/applications-module.h"
 #include "ns3/point-to-point-helper.h"
 #include "ns3/config-store.h"
 #include "ns3/electric-consumption-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("LteSimulationExample");

void RemainingEnergyTrace (std::string context, double previousEnergy, double currentEnergy)
{
  Ptr<Node> node = GetNodeFromContext(context);
  Ptr<ElectricVehicleConsumptionModel> consumptionModel = node->GetObject<ElectricVehicleConsumptionModel> ();
  Ptr<const MobilityModel> mobilityModel = consumptionModel->GetMobilityModel ();
  Vector pos = mobilityModel->GetPosition ();

  NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << "\t"
    << node->GetId () << "\t"
    << pos.x << "\t"
    << pos.y << "\t"
    << pos.z << "\t"
    << consumptionModel->GetVelocity () << "\t"
    << consumptionModel->GetEnergyFraction () << "\t"
    << currentEnergy << "\t"
    << consumptionModel->GetEnergyConsumed () << "\t"
    << consumptionModel->GetTotalEnergyConsumed ());
}

int 
main (int argc, char *argv[])
{
  NS_LOG_UNCOND ("Ejemplo de simulación con LTE");

  int numeroVehiculos = 5;

  Time interPacketInterval = MilliSeconds (100);
  bool useCa = false;
  bool disableDl = false;
  bool disableUl = false;
  bool disablePl = false;

  if (useCa)
  {
    Config::SetDefault ("ns3::LteHelper::UseCa", BooleanValue (useCa));
    Config::SetDefault ("ns3::LteHelper::NumberOfComponentCarriers", UintegerValue (2));
    Config::SetDefault ("ns3::LteHelper::EnbComponentCarrierManager", StringValue ("ns3::RrComponentCarrierManager"));
  }

  NodeContainer vehiculos;
  vehiculos.Create(numeroVehiculos);

  // Mobilidad --------------

  MobilityHelper mobility;
  // Posición inicial
  // Nodos distribuidos aleatoriamente sobre una recta -> AUTOPISTA
  Ptr<RandomRectanglePositionAllocator> positionAloc = CreateObject<RandomRectanglePositionAllocator> ();
  positionAloc->SetAttribute("X", StringValue("ns3::UniformRandomVariable[Min=0.0|Max=100.0]"));
  positionAloc->SetAttribute("Y", StringValue("ns3::ConstantRandomVariable[Constant=50.0]"));
  mobility.SetPositionAllocator(positionAloc);

  // Velocidad constante
  mobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
  mobility.Install(vehiculos);
  Ptr<UniformRandomVariable> rvar = CreateObject<UniformRandomVariable> ();
  // A cada vehículo una velocidad constante aleatoria en linea recta
  for (NodeContainer::Iterator i = vehiculos.Begin (); i != vehiculos.End (); ++i) 
  {
    Ptr<Node> vehiculo = (*i);
    double speed = rvar->GetValue(15, 25);
    vehiculo->GetObject<ConstantVelocityMobilityModel>()->SetVelocity (Vector(speed, 0, 0));
  }

  // Añadimos un modelo de consumo eléctrico a los vehículos
  std::string vehicleAttributesFile = "vehicleAttributes.xml";
  double updateTime = 1;
  ElectricConsumptionHelper electricMobility = ElectricConsumptionHelper (vehicleAttributesFile, updateTime);
  electricMobility.Install ();

  // Cada vez que se actualice el consumo que se llame a la funcion RemainingEnergyTrace
  Config::Connect ("/NodeList/*/$ns3::ElectricVehicleConsumptionModel/RemainingEnergy",
            MakeCallback (&RemainingEnergyTrace)); 
  
  //Simulator::Schedule (Seconds(1.0), &PrintPositions); // Cada segundo mostramos la posición de los nodos


  // Comunicaciones -----------

  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);

  Ptr<Node> pgw = epcHelper->GetPgwNode ();

  // Cremos el host remoto al que se conectaran los vehículos
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create (1);
  Ptr<Node> remoteHost = remoteHostContainer.Get (0);
  InternetStackHelper internet;
  internet.Install (remoteHostContainer);

   // Creamos Internet - El protocolo con el que se conectaran al host
  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2ph.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (10)));
  NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);
  Ipv4AddressHelper ipv4h;
  ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);
  // interface 0 is localhost, 1 is the p2p device
  Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress (1);

  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);

  // Creamos las antenas
  NodeContainer enbNodes;
  enbNodes.Create(1);
  // Posicion de la antena
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector(200, 20, 0));
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator(positionAlloc);
  mobility.Install(enbNodes);

  // Instalamos los dispositivos LTE en los nodos. UE en los vehículos y eNB en las antenas.
  NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);
  NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice (vehiculos);

  // Instalamos el protocolo IP en los UE de los vehículos para poder conectar por internet
  internet.Install (vehiculos);
  Ipv4InterfaceContainer ueIpIface;
  ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueLteDevs));
  // Assign IP address to UEs, and install applications
  // Asigna la dirección IP a cada UE e instala las aplicaciones
  for (uint32_t u = 0; u < vehiculos.GetN (); ++u)
    {
      Ptr<Node> ueNode = vehiculos.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }

  // Asigna cada UE(vehículo) a cada eNB(antena)
  for (uint16_t i = 0; i < numeroVehiculos; i++)
    {
      lteHelper->Attach (ueLteDevs.Get(i), enbLteDevs.Get(0));
      // side effect: the default EPS bearer will be activated
  }

  // Instala la aplicación server en host remoto
  // e instala la aplicación cliente en el UE (vehículo)
  uint16_t dlPort = 1100;
  uint16_t ulPort = 2000;
  uint16_t otherPort = 3000;
  ApplicationContainer clientApps;
  ApplicationContainer serverApps;

  for (uint32_t u = 0; u < vehiculos.GetN (); ++u)
    {
      if (!disableDl)
        {
          PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), dlPort));
          serverApps.Add (dlPacketSinkHelper.Install (vehiculos.Get(u)));

          UdpClientHelper dlClient (ueIpIface.GetAddress (u), dlPort);
          dlClient.SetAttribute ("Interval", TimeValue (interPacketInterval));
          dlClient.SetAttribute ("MaxPackets", UintegerValue (1000000));
          clientApps.Add (dlClient.Install (remoteHost));
        }

      if (!disableUl)
        {
          ++ulPort;
          PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), ulPort));
          serverApps.Add (ulPacketSinkHelper.Install (remoteHost));

          UdpClientHelper ulClient (remoteHostAddr, ulPort);
          ulClient.SetAttribute ("Interval", TimeValue (interPacketInterval));
          ulClient.SetAttribute ("MaxPackets", UintegerValue (1000000));
          clientApps.Add (ulClient.Install (vehiculos.Get(u)));
        }

      if (!disablePl && numeroVehiculos > 1)
        {
          ++otherPort;
          PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), otherPort));
          serverApps.Add (packetSinkHelper.Install (vehiculos.Get(u)));

          UdpClientHelper client (ueIpIface.GetAddress (u), otherPort);
          client.SetAttribute ("Interval", TimeValue (interPacketInterval));
          client.SetAttribute ("MaxPackets", UintegerValue (1000000));
          clientApps.Add (client.Install (vehiculos.Get ((u + 1) % numeroVehiculos)));
        }
    }

  serverApps.Start (MilliSeconds (10));
  clientApps.Start (MilliSeconds (40));
  lteHelper->EnableTraces ();

  Simulator::Stop (MilliSeconds (10000)); // La simulación durará 1 segundos
  Simulator::Run ();

  Simulator::Destroy ();

}
