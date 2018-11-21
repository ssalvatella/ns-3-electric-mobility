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

#ifndef ELECTRIC_VEHICLE_CONSUMPTION_MODEL_H
#define ELECTRIC_VEHICLE_CONSUMPTION_MODEL_H

#include "ns3/traced-value.h"
#include "consumption-model.h"
#include "ns3/nstime.h"
#include "ns3/event-id.h"
#include "ns3/mobility-module.h"

#define STANDARD_GRAVITY 9.80665
#define DENSITY_AIR 1.2041
#define JOULES_TO_WH 0.0002778

namespace ns3 {

/**
 * \ingroup consumption
 * \brief Model consumption of electric vehicle based on [1].
 *
 * The values ​​of the model are loaded by the corresponding help class from an XML.
 * The default values are set to zero.
 *
 * Energy consumed each update can be trace from the attribute RemainingEnergyWh.
 *
 * The model requires several parameters for simulate consumption:
 * - InitialEnergyWh, initial energy when start the simulation, in Wh
 * - VehicleMass, mass of the vehicle in Kg
 * - MaximumBatteryCapacity, maximum capacity of the vehicle battery, in Wh
 * - FrontSurfaceArea, front surface area of the vehicle, in m2
 * - AirDragCoefficient, air drag coefficient of the vehicle
 * - InternalMomentOfInertia, internal moment of intertia of the vehicle, in Kg * m2
 * - RadialDragCoefficient, radial drag coefficient of the vehicle
 * - RollDragCoefficient, roll drag coefficient of the vehicle
 * - ConstantPowerIntake, constant power intake of the vehicle (lights, air conditioner, etc.), in W
 * - PropulsionEfficiency, propulsion efficiency factor
 * - RecuperationEfficiency, recuperation efficiency factor
 *
 * References:
 * [1] Kurczveil, T., López, P.A., Schnieder, E., Implementation of an Energy Model and a Charging Infrastructure in SUMO. 
 *     
 */
  class ElectricVehicleConsumptionModel : public ConsumptionModel
  {
  public:

    static TypeId GetTypeId (void);

    ElectricVehicleConsumptionModel ();

    ~ElectricVehicleConsumptionModel (void);

    /**
     * This function is called every update interval defined in the corresponding
     * helper in order to update the consumption of the vehicle.
    */
    virtual void UpdateConsumption (void);

  private:

    /**
     * \returns Double with the difference of battery energy between a moment [k] and [k + 1]
     *
     * \brief Calculate the diferrence of battery energy. 
     */
    double CalculateEnergyDiff (void);

    /**
     * Save the position and velocity of the momento to use in the next update
     * consumption call.
    */
    void SaveLastPosAndVel (void);

    /**
     * \returns Double distance in meters between two position vectors.
     *
     * \brief Calculate the Euclidean distance between two vectors in meters.
     */
    double GetDistance (Vector u, Vector v);

    /**
     * \param Double velocity in m/s.
     * 
     * \returns Double distance in meters
     *
     * \brief Calculate the distance traveled by the vehicle since the last update from a speed.
     */
    double VelocityToDistance (double velocity);

    /**
     * \param Vector of a velocity
     * 
     * \returns Double steering angle of the vehicle in radians
     *
     * \brief Calculate the steering angle of the vehicle in radians
     */
    double GetAngle (Vector v);

    /**
     * \returns Double steering angle of the vehicle in radians
     *
     * Calculate the steering angle of the vehicle in radians
     */
    double GetAngleDiff (double angle1, double angle2);

    /*
    * Getters and Setters
    */
  public:

    double GetInitialEnergy (void) const;

    void SetInitialEnergy (double initialEnergyWh);

    double GetSupplyVoltage (void) const;

    double GetRemainingEnergy (void);

    void SetRemainingEnergy (double remainingEnergy);

    void DecreaseRemainingEnergy (double energyDecrease);

    void IncreaseRemainingEnergy (double energyIncrease);

    double GetTotalEnergyConsumed (void);

    void SetTotalEnergyConsumed (double energyConsumed);

    double GetEnergyConsumed (void);

    void SetEnergyConsumed (double energyConsumed);

    void IncreaseTotalEnergyConsumed (double energyConsumed);

    double GetEnergyFraction (void);

    double GetMaximunBatteryCapacity (void);

    void SetMaximunBatteryCapacity (double maximunBatteryCapacity);

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
    TracedValue<double> m_remainingEnergyWh;      // remaining energy in Wh
    double m_totalEnergyConsumed = 0;                 // total energy consumed in Wh
    double m_energyConsumed;                      // energy consumed in last update in Wh
    double m_maximumBatteryCapacity;              // maximum battery capacity in Wh
    double m_vehicleMass;                         // vehicle mass in Kg
    double m_frontSurfaceArea;                    // front surface area of vehicle in m2
    double m_airDragCoefficient;                  // air drag coefficient
    double m_internalMomentOfInertia;             // internal moment of inertia in Kg * m2
    double m_radialDragCoefficient;               // radial drag coefficicent
    double m_rollDragCoefficient;                 // roll drag coefficient
    double m_constantPowerIntake;                 // constant power intake of vehicle in W
    double m_propulsionEfficiency;                // propulsion efficiency factor
    double m_recuperationEfficiency;              // recuperation efficiency factor
    double m_lastAngle = 0;                           // last angle of vehicle in degrees
    Time m_timeFromLastUpdate;
  };

} // namespace ns3

#endif /* ELECTRIC_VEHICLE_CONSUMPTION_MODEL_H */