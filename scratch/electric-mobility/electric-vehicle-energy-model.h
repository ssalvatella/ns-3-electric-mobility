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

namespace ns3 {

  class ElectricVehicleEnergyModel : public EnergySource
  {
  public:

    static TypeId GetTypeId (void);

    ElectricVehicleEnergyModel ();

    ~ElectricVehicleEnergyModel (void);

    void UpdateEnergySource (void);

    /*
    * Getters and Setters
    */

    void SetMobilityModel (Ptr<const MobilityModel> mobilityModel);

    Ptr<const MobilityModel> GetMobilityModel (void);

    double GetInitialEnergy (void) const;

    void SetInitialEnergy (double initialEnergyWh);

    double GetSupplyVoltage (void) const;

    double GetRemainingEnergy (void);

    void SetRemainingEnergy (double remainingEnergy);

    void DecreaseRemainingEnergy (double energyDecrease);

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
    
  private:
    double m_initialEnergyWh;
    double m_remainingEnergyWh;
    double m_maximumBatteryCapacity;
    double m_maximumPower;
    double m_vehicleMass;
    double m_frontSurfaceArea;
    double m_airDragCoefficient;
    double m_internalMomentOfInertia;
    double m_radialDragCoefficient;
    double m_rollDragCoefficient;
    double m_constantPowerIntake;
    double m_propulsionEfficiency;
    double m_recuperationEfficiency;
    Ptr<const MobilityModel> m_mobilityModel;
    Time m_lastUpdateTime;
  };

} // namespace ns3

#endif /* ELECTRIC_VEHICLE_ENERGY_MODEL_H */