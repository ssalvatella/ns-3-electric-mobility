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

#ifndef CONSUMPTION_H
#define CONSUMPTION_H

#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/type-id.h"
#include "ns3/mobility-module.h"

namespace ns3
{

/**
 * \defgroup consumption Consumption Models
 *
 */    

/**
 * \ingroup consumption
 *
 * \brief Consumption model base class.
 *
 * This is the base class for consumption models. The consumption models perform
 * calculation of the consumption of a vehicle simulated by a mobility. Consumption
 * models update the energy or the fuel by implementing the update function depending
 * on the type of vehicle (electric, gasoil, gasoline, etc.).
 * 
 * The model uses the mobility of the node to perform the calculations keeping
 * the position and speed since the last update.
 *
 */

class ConsumptionModel : public Object
{
public:
    static TypeId GetTypeId (void);
    ConsumptionModel ();
    virtual ~ConsumptionModel ();

    /**
     * This function is called every update interval defined in the corresponding
     * helper in order to update the consumption of the vehicle.
    */
    virtual void UpdateConsumption (void) = 0;

    /**
     * \brief Sets pointer to node containing this ConsumptionModel.
     *
     * \param node Pointer to node containing this ConsumptionModel.
     */
    void SetNode (Ptr<Node> node);

    /**
     * \returns Pointer to node containing this ConsumptionModel.
     *
     * When a subclass needs to get access to the underlying node base class to
     * print the nodeId for example, it can invoke this method.
     */
    Ptr<Node> GetNode (void) const;

    /**
     * \brief Sets pointer to mobility content in the node.
     *
     * \param pointer to mobility containing in the node.
    */
    void SetMobilityModel (Ptr<const MobilityModel> mobilityModel);

    /**
     * \returns Pointer to mobility content in the node.
     *
     * When a subclass needs to get access to the underlying mobility base class to
     * get information about the mobility.
     */
    Ptr<const MobilityModel> GetMobilityModel (void);

    /**
     * \brief Sets vector of the last position.
     *
     * \param vector of the last position.
    */
    void SetLastPosition (Vector lastPosition);

    /**
     * \returns Vector of the last position from last update.
     *
     * Get the vector of the last positition from last update.
     */
    Vector GetLastPosition (void);

    /**
     * \brief Sets vector of the last velocity.
     *
     * \param vector of the last velocity.
    */
    void SetLastVelocity (Vector lastVelocity);

    /**
     * \returns Vector of the last velocity from last update.
     *
     * Get the vector of the last velocity from last update.
     */
    Vector GetLastVelocity (void);

    /**
     * \brief Sets the time of the last update.
     *
     * \param time of the last velocity.
    */
    void SetLastUpdateTime (Time lastUpdate);

    /**
     * \returns Time of the last update.
     *
     * Get the time of the last update.
     */
    Time GetLastUpdateTime (void);

private:

    /**
     * Pointer to node containing this Consumption Model. Used by helper class to make
     * sure models are installed onto the corresponding node.
     */
    Ptr<Node> m_node;

protected:

    /**
     * Pointer to mobility content in the node. Used for the update
     * for calculations of consumption.
     */
    Ptr<const MobilityModel> m_mobilityModel;

    /**
     * Vector of the last position (x, y, z).
     */
    Vector m_lastPosition;

    /**
     * Vector of the last velocity (x, y, z) expressed in m/s.
     */
    Vector m_lastVelocity;

    /**
     * Time of the last update.
     */
    Time m_lastUpdateTime;

};

}

#endif /* CONSUMPTION_H */
