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

#include <cmath>

#include "ns3/log.h"
#include "ns3/assert.h"
#include "ns3/double.h"
#include "ns3/trace-source-accessor.h"
#include "electric-vehicle-energy-model.h"
#include "ns3/simulator.h"

namespace ns3 {

    NS_LOG_COMPONENT_DEFINE ("ElectricVehicleEnergyModel");

    NS_OBJECT_ENSURE_REGISTERED (ElectricVehicleEnergyModel);

    TypeId
    ElectricVehicleEnergyModel::GetTypeId (void)
    {
      static TypeId tid = TypeId ("ns3::ElectricVehicleEnergyModel");
      return tid;
    }

    ElectricVehicleEnergyModel::ElectricVehicleEnergyModel ()
    {
      NS_LOG_FUNCTION (this);
    }

    ElectricVehicleEnergyModel::~ElectricVehicleEnergyModel ()
    {
      NS_LOG_FUNCTION (this);
    }    

    void
    ElectricVehicleEnergyModel::UpdateEnergySource (void)
    {

      NS_LOG_FUNCTION (this);
      NS_LOG_DEBUG ("ElectricVehicleEnergyModel:Updating remaining energy at node #" <<
                    GetNode ()->GetId ());     

      // do not update if simulation has finished
      if (Simulator::IsFinished ()) 
      {
        return;
      }

      DecreaseRemainingEnergy (0.5);

      m_lastUpdateTime = Simulator::Now ();

    }    

    double
    ElectricVehicleEnergyModel::GetInitialEnergy (void) const
    {
      NS_LOG_FUNCTION (this);
      return m_initialEnergyWh;
    }

    void
    ElectricVehicleEnergyModel::SetInitialEnergy (double initialEnergyWh)
    {
      NS_LOG_FUNCTION (this << initialEnergyWh);
      // check if initial energy isn't bigger than maximum capacity
      NS_ASSERT (initialEnergyWh >= 0);
      m_initialEnergyWh = initialEnergyWh;
      // set remaining energy to be initial energy
      m_remainingEnergyWh = initialEnergyWh;
    }

    double
    ElectricVehicleEnergyModel::GetSupplyVoltage (void) const
    {
      NS_LOG_FUNCTION (this);
      return 0;
    }

    double
    ElectricVehicleEnergyModel::GetRemainingEnergy (void)
    {
      NS_LOG_FUNCTION (this);
      return m_remainingEnergyWh;
    }    

    void
    ElectricVehicleEnergyModel::SetRemainingEnergy (double remainingEnergyWh)
    {

      NS_LOG_FUNCTION (this << remainingEnergyWh);
      m_remainingEnergyWh = remainingEnergyWh;
    }

    void
    ElectricVehicleEnergyModel::DecreaseRemainingEnergy (double energyDecrease)
    {
      NS_LOG_FUNCTION (this << energyDecrease);
      SetRemainingEnergy (GetRemainingEnergy() - energyDecrease);
    }    

    double
    ElectricVehicleEnergyModel::GetEnergyFraction (void)
    {
      NS_LOG_FUNCTION (this);
      return 0;
    }

}