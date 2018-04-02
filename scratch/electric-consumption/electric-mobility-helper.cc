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

  ElectricMobilityHelper::ElectricMobilityHelper (std::string filename, Ns2MobilityHelper ns2)
    : m_filename (filename), m_ns2 (ns2)
  {
    std::ifstream file (m_filename.c_str (), std::ios::in);
    if (!(file.is_open ())) NS_FATAL_ERROR("Could not open vehicule attributes file " << m_filename.c_str() << " for reading, aborting here \n");
  }

  // Prints actual position and velocity when a course change event occurs
  static
  void
  CourseChange (std::string context, Ptr<const MobilityModel> mobility)
  {
    // Get the node Id from the context of callback
    Ptr<Node> n = GetNodeFromContext(context);
    Ptr<ElectricVehicleConsumptionModel> consumptionModel = n->GetObject<ElectricVehicleConsumptionModel> ();
    consumptionModel->UpdateConsumption ();
  }

  void 
  ElectricMobilityHelper::Install (void)
  {

    CreateElectricVehicleConsumptionModels ();
    // Configure callback for logging
    Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",
                    MakeCallback (&CourseChange));
  }

/*
 * Private functions start here.
 */

  void
  ElectricMobilityHelper::CreateElectricVehicleConsumptionModels (void)
  {
    uint32_t i;
    for (i = 0; i < NodeList::GetNNodes (); i++) 
    {
      Ptr<Node> n = NodeList::GetNode (i);
      Ptr<ElectricVehicleConsumptionModel> consumptionModel = CreateObject<ElectricVehicleConsumptionModel> ();
      consumptionModel->SetNode(n);

      // All consumption model has a mobility model
      Ptr<MobilityModel> mobility = n->GetObject<MobilityModel> ();
      consumptionModel->SetMobilityModel (mobility);

      consumptionModel->SetVehicleMass (10000);
      consumptionModel->SetFrontSurfaceArea (6);
      consumptionModel->SetAirDragCoefficient (0.6);
      consumptionModel->SetInternalMomentOfInertia (0.01);
      consumptionModel->SetRadialDragCoefficient (0.5);
      consumptionModel->SetRollDragCoefficient (0.01);
      consumptionModel->SetConstantPowerIntake (100);
      consumptionModel->SetPropulsionEfficiency (0.9);
      consumptionModel->SetRecuperationEfficiency (0.9);
      consumptionModel->SetMaximunBatteryCapacity (24000);
      consumptionModel->SetInitialEnergy (24000);
      n->AggregateObject (consumptionModel);
    }
  }

  Ptr<Node> 
  GetNodeFromContext (std::string context)
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
          return NodeList::GetNode (std::stoi(token));
        }
    }
    return NULL;
  }  

} // namespace ns3