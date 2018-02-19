/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2018 Unizar
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Samuel Salvatella <ssalvatellaperez@gmail.com>
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "ns3/core-module.h"
#include "ns3/node-list.h"
#include "ns3/node.h"
#include "ns3/log.h"
#include "ns3/mobility-module.h"
#include "ns3/ns2-mobility-helper.h"
#include "electric-mobility-helper.h"

namespace ns3 {

  NS_LOG_COMPONENT_DEFINE ("ElectricMobilityHelper");

  ElectricMobilityHelper::ElectricMobilityHelper (std::string filename, Ns2MobilityHelper ns2, std::string logFile)
    : m_filename (filename), m_ns2 (ns2), m_logFile (logFile)
  {
    std::ifstream file (m_filename.c_str (), std::ios::in);
    if (!(file.is_open ())) NS_FATAL_ERROR("Could not open vehicule attributes file " << m_filename.c_str() << " for reading, aborting here \n");
  }

  // Prints actual position and velocity when a course change event occurs
  static
  void
  CourseChange (std::map<uint32_t, ElectricVehicleEnergyModel> *m_electricVehicleEnergyModels, std::string context, Ptr<const MobilityModel> mobility)
  {

    // Get the node Id from the context of callback
    uint32_t nodeId = GetNodeIdFromContext(context);
 
    Vector pos = mobility->GetPosition (); // Get position

    ElectricVehicleEnergyModel * energyModel = &((*(*m_electricVehicleEnergyModels).find(nodeId)).second);
    energyModel->SetMobilityModel (mobility);
    energyModel->UpdateEnergySource ();

    // Prints position, velocities and energy consumption
    std::cout << Simulator::Now ().GetMilliSeconds () 
        << "\tNode=" << nodeId 
        << "\tPOS: x=" << pos.x 
        << "\ty=" << pos.y
        << "\tz=" << pos.z 
        << ";\tVEL(m/s) = " << energyModel->GetVelocity ()  
        << "\tInitial Energy(Wh) = " << energyModel->GetInitialEnergy ()
        << "\tRemaining Energy(Wh) = " << energyModel->GetRemainingEnergy () 
        << "\tBattery Level(%) = " << energyModel->GetEnergyFraction () * 100
        << std::endl;
  }   

  void 
  ElectricMobilityHelper::Install (void)
  {

    // open log file for output
    std::ofstream os;
    os.open (m_logFile.c_str ());

    CreateElectricVehicleEnergyModels ();
    // Configure callback for logging
    Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",
                    MakeBoundCallback (&CourseChange, &m_electricVehicleEnergyModels));
  }

  std::map<uint32_t, ElectricVehicleEnergyModel>
  ElectricMobilityHelper::GetElectricVehicleEnergyModels (void)
  {
    return m_electricVehicleEnergyModels;
  }

/*
 * Private functions start here.
 */

  void
  ElectricMobilityHelper::CreateElectricVehicleEnergyModels (void)
  {
    uint32_t i;
    for (i = 0; i < NodeList::GetNNodes (); i++) 
    {
      Ptr<Node> n = NodeList::GetNode (i);
      ElectricVehicleEnergyModel energyModel = ElectricVehicleEnergyModel ();
      energyModel.SetNode(n);
      energyModel.SetVehicleMass (10000);
      energyModel.SetFrontSurfaceArea (6);
      energyModel.SetAirDragCoefficient (0.6);
      energyModel.SetInternalMomentOfInertia (0.01);
      energyModel.SetRadialDragCoefficient (0.5);
      energyModel.SetRollDragCoefficient (0.01);
      energyModel.SetConstantPowerIntake (100);
      energyModel.SetPropulsionEfficiency (0.9);
      energyModel.SetRecuperationEfficiency (0.9);
      energyModel.SetMaximunBatteryCapacity (24000);
      energyModel.SetInitialEnergy (24000);
      m_electricVehicleEnergyModels.insert ( std::pair<uint32_t,ElectricVehicleEnergyModel>(i,energyModel) );
    }
  }

  uint32_t 
  GetNodeIdFromContext (std::string context)
  {
    std::string delimiter = "/";

    size_t i = 0;
    size_t count = 0;
    std::string token;
    while ((i = context.find(delimiter)) != std::string::npos) {
        count++;
        token = context.substr(0, i);
        context.erase(0, i + delimiter.length());
        if (count == 3)
        {
          return std::stoi(token);
        }
    }
    return 0;
  }  

} // namespace ns3