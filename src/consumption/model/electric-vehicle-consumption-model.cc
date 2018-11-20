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
      
      double energyDiff = CalculateEnergyDiff (); // Wh
      
      DecreaseRemainingEnergy (energyDiff);
      SetEnergyConsumed (energyDiff);
      IncreaseTotalEnergyConsumed (energyDiff);

      SetLastUpdateTime (Simulator::Now ());
      SaveLastPosAndVel ();
      m_lastAngle = GetAngle (GetMobilityModel ()->GetVelocity ());
    }

    double ElectricVehicleConsumptionModel::CalculateEnergyDiff (void)
    {

      double velocityNow = GetVelocity (m_mobilityModel->GetVelocity ());
      double lastVelocity = GetVelocity (m_lastVelocity);
      double heightNow = m_mobilityModel->GetPosition ().z;
      double lastHeight = m_lastPosition.z;

      double distanceCovered = VelocityToDistance (velocityNow);

      // calculate potential energy difference
      double energyDiff = GetVehicleMass () * STANDARD_GRAVITY * (heightNow - lastHeight);

      // kinetic energy difference of vehicle
      energyDiff += 0.5 * GetVehicleMass () * (velocityNow * velocityNow - lastVelocity * lastVelocity);

      // add rotational energy diff of internal rotating elements
      energyDiff += GetInternalMomentOfInertia () * (velocityNow * velocityNow - lastVelocity * lastVelocity);

      // Energy loss through Air resistance [Ws]
      // Calculate energy losses:
      // EnergyLoss,Air = 1/2 * rho_air [kg/m^3] * myFrontSurfaceArea [m^2] * myAirDragCoefficient [-] * v_Veh^2 [m/s] * s [m]
      //                    ... with rho_air [kg/m^3] = 1,2041 kg/m^3 (at T = 20C)
      //                    ... with s [m] = v_Veh [m/s] * TS [s]
      energyDiff += 0.5 * DENSITY_AIR * GetFrontSurfaceArea () * GetAirDragCoefficient () * velocityNow * velocityNow * distanceCovered;

      // Energy loss through Roll resistance [Ws]
      //                    ... (fabs(veh.getSpeed())>=0.01) = 0, if vehicle isn't moving
      // EnergyLoss,Tire = c_R [-] * F_N [N] * s [m]
      //                    ... with c_R = ~0.012    (car tire on asphalt)
      //                    ... with F_N [N] = myMass [kg] * g [m/s^2]
      energyDiff += GetRollDragCoefficient () * STANDARD_GRAVITY * GetVehicleMass () * distanceCovered;

      // Energy loss through friction by radial force [Ws]
      // If angle of vehicle was changed
      const double angleDiff = m_lastAngle == std::numeric_limits<double>::infinity() ? 0. : GetAngleDiff(m_lastAngle, GetAngle (GetMobilityModel ()->GetVelocity ()));

      if (angleDiff != 0.)
      {
        // Compute new radio
        double radius = distanceCovered / fabs(angleDiff);

        // Check if radius is in the interval [0.0001 - 10000] (To avoid overflow and division by zero)
        if (radius < 0.0001)
        {
            radius = 0.0001;
        } else if (radius > 10000)
        {
            radius = 10000;
        }

        // EnergyLoss,internalFrictionRadialForce = c [m] * F_rad [N];
        // Energy loss through friction by radial force [Ws]
        energyDiff += GetRadialDragCoefficient () * GetVehicleMass () * velocityNow * velocityNow / radius;
      }

      // EnergyLoss,constantConsumers
      // Energy loss through constant loads (e.g. A/C) [Ws]
      energyDiff += GetConstantPowerIntake ();

      //E_Bat = E_kin_pot + EnergyLoss;
      if (energyDiff > 0)
      {
        energyDiff /= GetPropulsionEfficiency ();
      } else
      {
        energyDiff += GetRecuperationEfficiency ();
      }

      // convert from [Ws] to [Wh] (3600s / 1h):
      return energyDiff / 3600;
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

    double
    ElectricVehicleConsumptionModel::VelocityToDistance (double velocity)
    {
      return velocity * m_timeFromLastUpdate.GetSeconds ();
    }

    double
    ElectricVehicleConsumptionModel::GetDistance (Vector u, Vector v)
    {
      return std::sqrt ((std::pow (u.x - v.x, 2) + std::pow (u.y - v.y, 2) + std::pow (u.z - v.z, 2)));
    }

    double
    ElectricVehicleConsumptionModel::GetAngle (Vector v)
    {
      return std::atan2 (v.y, v.x);
    }

    double
    ElectricVehicleConsumptionModel::GetAngleDiff (double angle1, double angle2)
    {
      double dtheta = angle2 - angle1;
      while (dtheta > (double) M_PI) {
          dtheta -= (double)(2.0 * M_PI);
      }
      while (dtheta < (double) - M_PI) {
          dtheta += (double)(2.0 * M_PI);
      }
      return dtheta;
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

    double
    ElectricVehicleConsumptionModel::GetEnergyConsumed (void)
    {
      NS_LOG_FUNCTION (this);
      return m_energyConsumed;
    }

    void
    ElectricVehicleConsumptionModel::SetEnergyConsumed (double energyConsumed)
    {
      NS_LOG_FUNCTION (this << energyConsumed);
      m_energyConsumed = energyConsumed;
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