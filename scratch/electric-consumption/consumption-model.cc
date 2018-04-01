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

#include "ns3/log.h"
#include "ns3/ptr.h"
#include "ns3/assert.h"
#include "ns3/double.h"
#include "ns3/type-id.h"
#include "ns3/mobility-module.h"
#include "consumption-model.h"

namespace ns3
{

    NS_LOG_COMPONENT_DEFINE ("ConsumptionModel");

    NS_OBJECT_ENSURE_REGISTERED (ConsumptionModel);

    TypeId
    ConsumptionModel::GetTypeId (void)
    {
        static TypeId tid = TypeId ("ns3::ConsumptionModel")
        .SetParent<Object> ()
        .SetGroupName ("Consumption")
        ;
        return tid;
    }

    ConsumptionModel::ConsumptionModel ()
    {
        NS_LOG_FUNCTION (this);
    }

    ConsumptionModel::~ConsumptionModel ()
    {
        NS_LOG_FUNCTION (this);
    }

    void
    ConsumptionModel::BindMobility ()
    {
        NS_LOG_FUNCTION (this);
        
    }

    void
    ConsumptionModel::SetNode (Ptr<Node> node)
    {
        NS_LOG_FUNCTION (this);
        NS_ASSERT (node != NULL);
        m_node = node;
    }
    
    Ptr<Node>
    ConsumptionModel::GetNode (void) const
    {
        return m_node;
    }

    Ptr<const MobilityModel> 
    ConsumptionModel::GetMobilityModel (void)
    {
        return m_mobilityModel;
    }

    void 
    ConsumptionModel::SetMobilityModel (Ptr<const MobilityModel> model)
    {
        NS_LOG_FUNCTION (this);
        NS_ASSERT (model != NULL);
        m_mobilityModel = model;
    }

    void
    ConsumptionModel::SetLastPosition (Vector lastPosition)
    {
        NS_LOG_FUNCTION (this);
        m_lastPosition = lastPosition;
    }

    Vector
    ConsumptionModel::GetLastPosition (void)
    {
        return m_lastPosition;
    }

    void
    ConsumptionModel::SetLastVelocity (Vector lastVelocity)
    {
        NS_LOG_FUNCTION (this);
        m_lastVelocity = lastVelocity;
    }

    Vector
    ConsumptionModel::GetLastVelocity (void)
    {
        return m_lastVelocity;
    }

    void
    ConsumptionModel::SetLastUpdateTime (Time lastUpdate)
    {
        NS_LOG_FUNCTION (this);
        m_lastUpdateTime = lastUpdate;
    }

    Time
    ConsumptionModel::GetLastUpdateTime (void)
    {
        return m_lastUpdateTime;
    }

}