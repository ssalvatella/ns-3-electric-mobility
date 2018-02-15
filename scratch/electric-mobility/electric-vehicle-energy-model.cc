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

    /*
    * Getters and Setters
    */

    double 
    ElectricVehicleEnergyModel::GetVelocity (void)
    {
      Vector vel = m_mobilityModel->GetVelocity ();
      return std::sqrt (std::pow(vel.x, 2) + std::pow(vel.y, 2) + std::pow(vel.z, 2));
    }

    Ptr<const MobilityModel> 
    ElectricVehicleEnergyModel::GetMobilityModel (void)
    {
      return m_mobilityModel;
    }

    void 
    ElectricVehicleEnergyModel::SetMobilityModel (Ptr<const MobilityModel> mobilityModel)
    {
      m_mobilityModel = mobilityModel;
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
      return GetRemainingEnergy () / GetMaximunBatteryCapacity ();
    }

    double
    ElectricVehicleEnergyModel::GetMaximunBatteryCapacity (void)
    {
      return m_maximumBatteryCapacity;
    }

    void 
    ElectricVehicleEnergyModel::SetMaximunBatteryCapacity (double maximunBatteryCapacity)
    {
      m_maximumBatteryCapacity = maximunBatteryCapacity;
    }

    double
    ElectricVehicleEnergyModel::GetMaximumPower (void)
    {
      return m_maximumPower;
    }

    void
    ElectricVehicleEnergyModel::SetMaximumPower (double maximumPower)
    {
      m_maximumPower = maximumPower;
    }

    double
    ElectricVehicleEnergyModel::GetVehicleMass (void)
    {
      return m_vehicleMass;
    }

    void
    ElectricVehicleEnergyModel::SetVehicleMass (double vehicleMass)
    {
      m_vehicleMass = vehicleMass;
    }

    double
    ElectricVehicleEnergyModel::GetFrontSurfaceArea (void)
    {
      return m_frontSurfaceArea;
    }

    void
    ElectricVehicleEnergyModel::SetFrontSurfaceArea (double frontSurfaceArea)
    {
      m_frontSurfaceArea = frontSurfaceArea;
    }

    double
    ElectricVehicleEnergyModel::GetAirDragCoefficient (void)
    {
      return m_airDragCoefficient;
    }

    void 
    ElectricVehicleEnergyModel::SetAirDragCoefficient (double airDragCoefficient)
    {
      m_airDragCoefficient = airDragCoefficient;
    }

    double
    ElectricVehicleEnergyModel::GetInternalMomentOfInertia (void)
    {
      return m_internalMomentOfInertia;
    }

    void
    ElectricVehicleEnergyModel::SetInternalMomentOfInertia (double internalMomentOfInertia)
    {
      m_internalMomentOfInertia = internalMomentOfInertia;
    }

    double
    ElectricVehicleEnergyModel::GetRadialDragCoefficient (void)
    {
      return m_radialDragCoefficient;
    }

    void
    ElectricVehicleEnergyModel::SetRadialDragCoefficient (double radialDragCoefficient)
    {
      m_radialDragCoefficient = radialDragCoefficient;
    }

    double
    ElectricVehicleEnergyModel::GetRollDragCoefficient (void)
    {
      return m_rollDragCoefficient;
    }

    void
    ElectricVehicleEnergyModel::SetRollDragCoefficient (double rollDragCoefficient)
    {
      m_rollDragCoefficient = rollDragCoefficient;
    }  

    double
    ElectricVehicleEnergyModel::GetPropulsionEfficiency (void)
    {
      return m_propulsionEfficiency;
    }

    void
    ElectricVehicleEnergyModel::SetPropulsionEfficiency (double propulsionEfficiency)
    {
      m_propulsionEfficiency = propulsionEfficiency;
    }

    double
    ElectricVehicleEnergyModel::GetRecuperationEfficiency (void)
    {
      return m_recuperationEfficiency;
    }

    void
    ElectricVehicleEnergyModel::SetRecuperationEfficiency (double recuperationEfficiency)
    {
      m_recuperationEfficiency = recuperationEfficiency;
    }

}