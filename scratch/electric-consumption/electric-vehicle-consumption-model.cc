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
#include "electric-vehicle-consumption-model.h"
#include "ns3/simulator.h"

namespace ns3 {

    NS_LOG_COMPONENT_DEFINE ("ElectricVehicleConsumptionModel");

    NS_OBJECT_ENSURE_REGISTERED (ElectricVehicleConsumptionModel);

    TypeId
    ElectricVehicleConsumptionModel::GetTypeId (void)
    {
      static TypeId tid = TypeId ("ns3::ElectricVehicleConsumptionModel")
        .SetParent<ConsumptionModel> ()
        .SetGroupName ("Consumption")
        .AddConstructor<ElectricVehicleConsumptionModel> ()
        .AddTraceSource ("RemainingEnergy",
                        "Remaining energy in vehicle in Wh.",
                        MakeTraceSourceAccessor (&ElectricVehicleConsumptionModel::m_remainingEnergyWh),
                        "ns3::TracedValueCallback::Double")
      ;
      return tid;
    }

    ElectricVehicleConsumptionModel::ElectricVehicleConsumptionModel ()
    {
      NS_LOG_FUNCTION (this);
    }

    ElectricVehicleConsumptionModel::~ElectricVehicleConsumptionModel ()
    {
      NS_LOG_FUNCTION (this);
    }

    void
    ElectricVehicleConsumptionModel::UpdateConsumption (void)
    {

      m_timeFromLastUpdate = Simulator::Now () - GetLastUpdateTime ();

      NS_LOG_FUNCTION (this);
      NS_LOG_DEBUG ("ElectricVehicleConsumptionModel:Updating remaining energy at node #" <<
                    GetNode ()->GetId ());     

      // do not update if simulation has finished
      if (Simulator::IsFinished ()) 
      {
        return;
      }
      
      double energyGain = CalculateEnergyGain () * JOULES_TO_WH;
      IncreaseRemainingEnergy (energyGain);
      IncreaseTotalEnergyConsumed (-energyGain);

      SetLastUpdateTime (Simulator::Now ());
      SaveLastPosAndVel ();
    }

    double ElectricVehicleConsumptionModel::CalculateEnergyGain (void)
    {
      double velocityNow = GetVelocity (m_mobilityModel->GetVelocity ());
      double lastVelocity = GetVelocity (m_lastVelocity);
      double heightNow = m_mobilityModel->GetPosition ().z;
      double lastHeight = m_lastPosition.z;

/*       std::cout << "velocityNow: " << velocityNow << "\n";
      std::cout << "heightNow: " << heightNow << "\n";
      std::cout << "lastVelocity: " << lastVelocity << "\n";
      std::cout << "lastHeight: " << lastHeight << "\n";
      std::cout << "lastPos: " << m_lastPosition << "\n"; */

      double vehicleEnergyNow = CalculateVehicleEnergy (velocityNow, heightNow);
      double vehicleEnergyLast = CalculateVehicleEnergy (lastVelocity, lastHeight);
      double energyLoss = CalculateEnergyLoss (lastVelocity, 1); // dirty distanceCovered, must be changed

      // std::cout << "vehicleEnergyNow: "  << vehicleEnergyNow * JOULES_TO_WH << "\t"
      //           << "vehicleEnergyLast: " << vehicleEnergyLast * JOULES_TO_WH << "\t"
      //           << "energyLoss: " << energyLoss * JOULES_TO_WH << "\t"
      //           << "totalEnergyGain: " << (vehicleEnergyNow - vehicleEnergyLast - energyLoss) * JOULES_TO_WH << "\n";

      return vehicleEnergyNow
             - vehicleEnergyLast
             - energyLoss;
    }

    double
    ElectricVehicleConsumptionModel::CalculateEnergyLoss (double velocity, double distanceCovered)
    {
      NS_LOG_FUNCTION (this << velocity << " " << distanceCovered);
      return CalculateLossEnergyAir (velocity, distanceCovered) + CalculateLossEnergyRolling (distanceCovered) 
        + CalculateLossEnergyCurve (velocity, distanceCovered) + CalculateLossEnergyConst ();
    }

    double
    ElectricVehicleConsumptionModel::CalculateLossEnergyAir (double velocity, double distanceCovered)
    {
      NS_LOG_FUNCTION (this << velocity << " " << distanceCovered);
      return (DENSITY_AIR / 2) * GetFrontSurfaceArea () * GetAirDragCoefficient () * std::pow (velocity, 2) * distanceCovered;
    }

    double
    ElectricVehicleConsumptionModel::CalculateLossEnergyRolling (double distanceCovered)
    {
      NS_LOG_FUNCTION (this << distanceCovered);
      return GetRollDragCoefficient () * GetVehicleMass () * STANDARD_GRAVITY * distanceCovered;
    }

    double
    ElectricVehicleConsumptionModel::CalculateLossEnergyCurve (double velocity, double distanceCovered)
    {
      NS_LOG_FUNCTION (this << velocity << " " << distanceCovered);
      // (m * v 2) / r [k] must change 1 by real value
      return GetRadialDragCoefficient () * ((GetVehicleMass () * std::pow (velocity, 2) / 1)) * distanceCovered;
    }

    double
    ElectricVehicleConsumptionModel::CalculateLossEnergyConst (void) {
      NS_LOG_FUNCTION (this);
      return GetConstantPowerIntake () * m_timeFromLastUpdate.GetSeconds ();
    }

    double
    ElectricVehicleConsumptionModel::CalculateVehicleEnergy (double velocity, double height)
    {
      NS_LOG_FUNCTION (this << velocity << " " << height) ;
      return CalculateKineticEnergy (velocity) + CalculatePotentialEnergy (height) + CalculateRotatingInertiaEnergy (velocity);
    }

    double
    ElectricVehicleConsumptionModel::CalculateKineticEnergy (double velocity)
    {
      NS_LOG_FUNCTION (this << velocity);
      return (GetVehicleMass () / 2) * std::pow (velocity, 2);
    }

    double
    ElectricVehicleConsumptionModel::CalculatePotentialEnergy (double height)
    {
      NS_LOG_FUNCTION (this);
      return GetVehicleMass () * height * STANDARD_GRAVITY;
    }

    double
    ElectricVehicleConsumptionModel::CalculateRotatingInertiaEnergy (double velocity)
    {
      NS_LOG_FUNCTION (this);
      return (GetInternalMomentOfInertia () / 2) * std::pow (velocity, 2);
    }

    void
    ElectricVehicleConsumptionModel::SaveLastPosAndVel (void)
    {
      m_lastPosition.x = m_mobilityModel->GetPosition ().x;
      m_lastPosition.y = m_mobilityModel->GetPosition ().y;
      m_lastPosition.z = m_mobilityModel->GetPosition ().z;

      m_lastVelocity.x = m_mobilityModel->GetVelocity ().x;
      m_lastVelocity.y = m_mobilityModel->GetVelocity ().y;
      m_lastVelocity.z = m_mobilityModel->GetVelocity ().z;
    }

    /*
    * Getters and Setters
    */

    double 
    ElectricVehicleConsumptionModel::GetVelocity (void)
    {
      Vector vel = m_mobilityModel->GetVelocity ();
      return std::sqrt (std::pow(vel.x, 2) + std::pow(vel.y, 2) + std::pow(vel.z, 2));
    }

    double 
    ElectricVehicleConsumptionModel::GetVelocity (Vector vel)
    {
      vel = m_mobilityModel->GetVelocity ();
      return std::sqrt (std::pow(vel.x, 2) + std::pow(vel.y, 2) + std::pow(vel.z, 2));
    }

    double
    ElectricVehicleConsumptionModel::GetInitialEnergy (void) const
    {
      NS_LOG_FUNCTION (this);
      return m_initialEnergyWh;
    }

    void
    ElectricVehicleConsumptionModel::SetInitialEnergy (double initialEnergyWh)
    {
      NS_LOG_FUNCTION (this << initialEnergyWh);
      // check if initial energy isn't bigger than maximum capacity
      NS_ASSERT (initialEnergyWh >= 0);
      m_initialEnergyWh = initialEnergyWh;
      // set remaining energy to be initial energy
      m_remainingEnergyWh = initialEnergyWh;
    }

    double
    ElectricVehicleConsumptionModel::GetSupplyVoltage (void) const
    {
      NS_LOG_FUNCTION (this);
      return 0;
    }

    double
    ElectricVehicleConsumptionModel::GetRemainingEnergy (void)
    {
      NS_LOG_FUNCTION (this);
      return m_remainingEnergyWh;
    }    

    void
    ElectricVehicleConsumptionModel::SetRemainingEnergy (double remainingEnergyWh)
    {

      NS_LOG_FUNCTION (this << remainingEnergyWh);
      m_remainingEnergyWh = remainingEnergyWh;
    }

    void
    ElectricVehicleConsumptionModel::DecreaseRemainingEnergy (double energyDecrease)
    {
      NS_LOG_FUNCTION (this << energyDecrease);
      SetRemainingEnergy (GetRemainingEnergy() - energyDecrease);
    }    

    void
    ElectricVehicleConsumptionModel::IncreaseRemainingEnergy (double energyIncrease)
    {
      NS_LOG_FUNCTION (this << energyIncrease);
      SetRemainingEnergy (GetRemainingEnergy() + energyIncrease);
    }

    double
    ElectricVehicleConsumptionModel::GetTotalEnergyConsumed (void)
    {
      NS_LOG_FUNCTION (this);
      return m_totalEnergyConsumed;
    }

    void
    ElectricVehicleConsumptionModel::SetTotalEnergyConsumed (double energyConsumed)
    {
      NS_LOG_FUNCTION (this << energyConsumed);
      m_totalEnergyConsumed = energyConsumed;
    }

    void
    ElectricVehicleConsumptionModel::IncreaseTotalEnergyConsumed (double energyConsumed)
    {
      NS_LOG_FUNCTION (this << energyConsumed);
      SetTotalEnergyConsumed (GetTotalEnergyConsumed () + energyConsumed);
    }

    double
    ElectricVehicleConsumptionModel::GetEnergyFraction (void)
    {
      NS_LOG_FUNCTION (this);
      return GetRemainingEnergy () / GetMaximunBatteryCapacity ();
    }

    double
    ElectricVehicleConsumptionModel::GetMaximunBatteryCapacity (void)
    {
      return m_maximumBatteryCapacity;
    }

    void 
    ElectricVehicleConsumptionModel::SetMaximunBatteryCapacity (double maximunBatteryCapacity)
    {
      m_maximumBatteryCapacity = maximunBatteryCapacity;
    }

    double
    ElectricVehicleConsumptionModel::GetMaximumPower (void)
    {
      return m_maximumPower;
    }

    void
    ElectricVehicleConsumptionModel::SetMaximumPower (double maximumPower)
    {
      m_maximumPower = maximumPower;
    }

    double
    ElectricVehicleConsumptionModel::GetVehicleMass (void)
    {
      return m_vehicleMass;
    }

    void
    ElectricVehicleConsumptionModel::SetVehicleMass (double vehicleMass)
    {
      m_vehicleMass = vehicleMass;
    }

    double
    ElectricVehicleConsumptionModel::GetFrontSurfaceArea (void)
    {
      return m_frontSurfaceArea;
    }

    void
    ElectricVehicleConsumptionModel::SetFrontSurfaceArea (double frontSurfaceArea)
    {
      m_frontSurfaceArea = frontSurfaceArea;
    }

    double
    ElectricVehicleConsumptionModel::GetAirDragCoefficient (void)
    {
      return m_airDragCoefficient;
    }

    void 
    ElectricVehicleConsumptionModel::SetAirDragCoefficient (double airDragCoefficient)
    {
      m_airDragCoefficient = airDragCoefficient;
    }

    double
    ElectricVehicleConsumptionModel::GetInternalMomentOfInertia (void)
    {
      return m_internalMomentOfInertia;
    }

    void
    ElectricVehicleConsumptionModel::SetInternalMomentOfInertia (double internalMomentOfInertia)
    {
      m_internalMomentOfInertia = internalMomentOfInertia;
    }

    double
    ElectricVehicleConsumptionModel::GetRadialDragCoefficient (void)
    {
      return m_radialDragCoefficient;
    }

    void
    ElectricVehicleConsumptionModel::SetRadialDragCoefficient (double radialDragCoefficient)
    {
      m_radialDragCoefficient = radialDragCoefficient;
    }

    double
    ElectricVehicleConsumptionModel::GetRollDragCoefficient (void)
    {
      return m_rollDragCoefficient;
    }

    void
    ElectricVehicleConsumptionModel::SetRollDragCoefficient (double rollDragCoefficient)
    {
      m_rollDragCoefficient = rollDragCoefficient;
    }  

    double
    ElectricVehicleConsumptionModel::GetPropulsionEfficiency (void)
    {
      return m_propulsionEfficiency;
    }

    void
    ElectricVehicleConsumptionModel::SetPropulsionEfficiency (double propulsionEfficiency)
    {
      m_propulsionEfficiency = propulsionEfficiency;
    }

    double
    ElectricVehicleConsumptionModel::GetRecuperationEfficiency (void)
    {
      return m_recuperationEfficiency;
    }

    void
    ElectricVehicleConsumptionModel::SetRecuperationEfficiency (double recuperationEfficiency)
    {
      m_recuperationEfficiency = recuperationEfficiency;
    }

    double
    ElectricVehicleConsumptionModel::GetConstantPowerIntake (void)
    {
      return m_constantPowerIntake;
    }

    void
    ElectricVehicleConsumptionModel::SetConstantPowerIntake(double constantPowerIntake)
    {
      m_constantPowerIntake = constantPowerIntake;
    }

}