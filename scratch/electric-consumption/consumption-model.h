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

class ConsumptionModel : public Object
{
public:
    static TypeId GetTypeId (void);
    ConsumptionModel ();
    virtual ~ConsumptionModel ();

    virtual void UpdateConsumption (void) = 0;

    void BindMobility (void);

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

    void SetMobilityModel (Ptr<const MobilityModel> mobilityModel);

    Ptr<const MobilityModel> GetMobilityModel (void);

    void SetLastPosition (Vector lastPosition);

    Vector GetLastPosition (void);

    void SetLastVelocity (Vector lastVelocity);

    Vector GetLastVelocity (void);

    void SetLastUpdateTime (Time lastUpdate);

    Time GetLastUpdateTime (void);

private:

    Ptr<Node> m_node;

protected:    
    Ptr<const MobilityModel> m_mobilityModel;     // pointer to mobility model
    Vector m_lastPosition;
    Vector m_lastVelocity;
    Time m_lastUpdateTime;

};

}

#endif /* CONSUMPTION_H */
