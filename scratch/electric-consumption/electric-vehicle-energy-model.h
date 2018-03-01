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

#ifndef ELECTRIC_VEHICLE_ENERGY_MODEL_H
#define ELECTRIC_VEHICLE_ENERGY_MODEL_H

#include "ns3/traced-value.h"
#include "ns3/energy-source.h"
#include "ns3/nstime.h"
#include "ns3/event-id.h"
#include "ns3/mobility-module.h"

#define STANDARD_GRAVITY 9.80665
#define DENSITY_AIR 1.225
#define JOULES_TO_WH 0.00028

namespace ns3 {

  class ElectricVehicleEnergyModel : public EnergySource
  {
  public:

    static TypeId GetTypeId (void);

    ElectricVehicleEnergyModel ();

    ~ElectricVehicleEnergyModel (void);

    void UpdateEnergySource (void);

  private:

    double CalculateEnergyGain (void);

    double CalculateVehicleEnergy (double velocity, double height);

    double CalculateEnergyLoss (double velocity, double distanceCovered);

    double CalculateLossEnergyAir (double velocity, double distanceCovered);

    double CalculateLossEnergyRolling (double distanceCovered);

    double CalculateLossEnergyCurve (double velocity, double distanceCovered);

    double CalculateLossEnergyConst (void);

    double CalculateKineticEnergy (double velocity);

    double CalculatePotentialEnergy (double height);

    double CalculateRotatingInertiaEnergy (double velocity);

    void SaveLastPosAndVel (void);

    /*
    * Getters and Setters
    */
  public:

    void SetMobilityModel (Ptr<const MobilityModel> mobilityModel);

    Ptr<const MobilityModel> GetMobilityModel (void);

    double GetInitialEnergy (void) const;

    void SetInitialEnergy (double initialEnergyWh);

    double GetSupplyVoltage (void) const;

    double GetRemainingEnergy (void);

    void SetRemainingEnergy (double remainingEnergy);

    void DecreaseRemainingEnergy (double energyDecrease);

    void IncreaseRemainingEnergy (double energyIncrease);

    double GetEnergyFraction (void);

    double GetMaximunBatteryCapacity (void);

    void SetMaximunBatteryCapacity (double maximunBatteryCapacity);

    double GetMaximumPower (void);

    void SetMaximumPower (double maximumPower);

    double GetVehicleMass (void);

    void SetVehicleMass (double vehicleMass);

    double GetFrontSurfaceArea (void);

    void SetFrontSurfaceArea (double frontSurfaceArea);

    double GetAirDragCoefficient (void);

    void SetAirDragCoefficient (double airDragCoefficient);

    double GetInternalMomentOfInertia (void);

    void SetInternalMomentOfInertia (double internalMomentOfInertia);

    double GetRadialDragCoefficient (void);

    void SetRadialDragCoefficient (double radialDragCoefficient);

    double GetRollDragCoefficient (void);

    void SetRollDragCoefficient (double rollDragCoefficient);

    double GetConstantPowerIntake (void);

    void SetConstantPowerIntake(double constantPowerIntake);

    double GetPropulsionEfficiency (void);

    void SetPropulsionEfficiency (double propulsionEfficiency);

    double GetRecuperationEfficiency (void);

    void SetRecuperationEfficiency (double recuperationEfficiency);

    double GetVelocity (void);

    double GetVelocity (Vector vel);
    
  private:
    double m_initialEnergyWh;                     // initial energy in Wh
    double m_remainingEnergyWh;                   // remaining energy in Wh
    double m_maximumBatteryCapacity;              // maximum battery capacity in Wh
    double m_maximumPower;                        // maximum power in W
    double m_vehicleMass;                         // vehicle mass in Kg
    double m_frontSurfaceArea;                    // front surface area of vehicle in m2
    double m_airDragCoefficient;                  // air drag coefficient
    double m_internalMomentOfInertia;             // internal moment of inertia in Kg * m2
    double m_radialDragCoefficient;               // radial drag coefficicent
    double m_rollDragCoefficient;                 // roll drag coefficient
    double m_constantPowerIntake;                 // constant power intake of vehicle in W
    double m_propulsionEfficiency;                // propulsion efficiency factor
    double m_recuperationEfficiency;              // recuperation efficiency factor 
    Ptr<const MobilityModel> m_mobilityModel;     // pointer to mobility model
    Vector m_lastPosition;
    Vector m_lastVelocity;
    Time m_lastUpdateTime;
    Time m_timeFromLastUpdate;
  };

} // namespace ns3

#endif /* ELECTRIC_VEHICLE_ENERGY_MODEL_H */