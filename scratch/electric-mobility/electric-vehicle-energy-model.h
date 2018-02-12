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

#include "ns3/energy-source.h"

namespace ns3 {

  class ElectricVehicleEnergyModel : public EnergySource
  {
  public:

    static TypeId GetTypeId (void);

    ElectricVehicleEnergyModel ();

    virtual ~ElectricVehicleEnergyModel (void);

    virtual UpdateEnergyConsumption (void);
    
  }

} // namespace ns3

#endif /* ELECTRIC_VEHICLE_ENERGY_MODEL_H */