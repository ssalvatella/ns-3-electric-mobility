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

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include "ns3/core-module.h"
#include "ns3/node-list.h"
#include "ns3/node.h"
#include "ns3/log.h"
#include "ns3/mobility-module.h"
#include "electric-mobility-helper.h"

namespace ns3 {

  NS_LOG_COMPONENT_DEFINE ("ElectricMobilityHelper");

  ElectricMobilityHelper::ElectricMobilityHelper (std::string filename, double updateTime)
    : m_filename (filename)
  {
    std::ifstream file (m_filename.c_str (), std::ios::in);
    if (updateTime <= 0 || std::isnan (updateTime))
    {
      m_updateTime = 1;
    } else
    {
      m_updateTime = updateTime;
    }
    NS_LOG_UNCOND ("update time " << m_updateTime);
    if (!(file.is_open ())) NS_FATAL_ERROR("Could not open vehicule attributes file " << m_filename.c_str() << " for reading, aborting here \n");
  }

  // Prints actual position and velocity when a course change event occurs
/*   static
  void
  CourseChange (std::string context, Ptr<const MobilityModel> mobility)
  {
    // Get the node Id from the context of callback
    Ptr<Node> n = GetNodeFromContext(context);
    Ptr<ElectricVehicleConsumptionModel> consumptionModel = n->GetObject<ElectricVehicleConsumptionModel> ();
    consumptionModel->UpdateConsumption ();
  } */

  void 
  ElectricMobilityHelper::Install (void)
  {
    LoadXml ();
    // Configure callback for logging
    //Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",
                    //MakeCallback (&CourseChange));
  }

  static void
  UpdateModelConsumption (Ptr<ElectricVehicleConsumptionModel> consumptionModel, double updateTime)
  {
    consumptionModel->UpdateConsumption ();
    Simulator::Schedule (Seconds (updateTime), &UpdateModelConsumption, consumptionModel, updateTime);
  }

/*
 * Private functions start here.
 */

  void
  ElectricMobilityHelper::LoadXml (void)
  {
    m_xmlDoc = NULL;
    /*parse the file and get the DOM */
    m_xmlDoc = xmlReadFile(m_filename.c_str (), NULL, 0);

    // case of failure
    if (m_xmlDoc == NULL)
    {
      NS_LOG_ERROR ("Could not parse XML file " << m_filename);
      return;
    }

    xmlNode *rootElement = xmlDocGetRootElement (m_xmlDoc);
    xmlNode *firstVehicle = rootElement->children;
    xmlNode *currentNode = NULL;

    // create a consumption model for each vehicle especified in XML
    for (currentNode = firstVehicle; currentNode; currentNode = currentNode->next) {
        if (currentNode->type == XML_ELEMENT_NODE) {
            CreateModelFromXml (currentNode);
        }
    }

    /*free the document */
    xmlFreeDoc(m_xmlDoc);

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();

  }

  void
  ElectricMobilityHelper::CreateModelFromXml (xmlNode * xmlVehicleNode)
  {
    xmlNode *currentNode = NULL;

    Ptr<ElectricVehicleConsumptionModel> consumptionModel = CreateObject<ElectricVehicleConsumptionModel> ();
    uint32_t nodeId;

    // read the node id of vehicle
    for(xmlAttrPtr attr = xmlVehicleNode->properties; NULL != attr; attr = attr->next)
    {
      std::string name = Convert (attr->name);
      if (name == "node")
      {  
        nodeId = std::stoi(Convert (attr->children->content));
      } else {
        NS_LOG_ERROR ("Node ID of vehicle is not defined.");
        return;
      }
    }

    Ptr<Node> node = NodeList::GetNode (nodeId);
    if (node == NULL) {
      NS_LOG_ERROR ("Node " << nodeId << " is not created in the simulation.");
      return;
    }

    // read parameters of electric vehicle
    for (currentNode = xmlVehicleNode->children; currentNode; currentNode = currentNode->next) {
      if (currentNode->type == XML_ELEMENT_NODE) {
        std::string clave;
        std::string valor;
        for(xmlAttrPtr attr = currentNode->properties; NULL != attr; attr = attr->next)
        {
          std::string name = Convert (attr->name);
          if (name == "key")
          {
            clave = Convert (attr->children->content);
          } else if (name == "value")
          {
            valor = Convert (attr->children->content);
          } else
          {
            NS_LOG_ERROR ("Could not parse XML file. Check the XML structure.");
            return;
          }

        }
        if (clave.empty () || valor.empty ())
        {
          NS_LOG_ERROR ("Could not parse XML file. Check the XML structure.");
          return;
        }

        // std::cout << clave << "=" << valor << "\n";

        if (clave == "maximumBatteryCapacity")
        {
          consumptionModel->SetMaximunBatteryCapacity (std::stod (valor));
        } else if (clave == "vehicleMass")
        {
          consumptionModel->SetVehicleMass (std::stod (valor));
        } else if (clave == "frontSurfaceArea")
        {
          consumptionModel->SetFrontSurfaceArea (std::stod (valor));
        } else if (clave == "airDragCoefficient")
        {
          consumptionModel->SetAirDragCoefficient (std::stod (valor));
        } else if (clave == "internalMomentOfInertia")
        {
          consumptionModel->SetInternalMomentOfInertia (std::stod (valor));
        } else if (clave == "radialDragCoefficient")
        {
          consumptionModel->SetRadialDragCoefficient (std::stod (valor));
        } else if (clave == "rollDragCoefficient")
        {
          consumptionModel->SetRollDragCoefficient (std::stod (valor));
        } else if (clave == "constantPowerIntake")
        {
          consumptionModel->SetConstantPowerIntake (std::stod (valor));
        } else if (clave == "propulsionEfficiency")
        {
          consumptionModel->SetPropulsionEfficiency (std::stod (valor));
        } else if (clave == "recuperationEfficiency")
        {
          consumptionModel->SetRecuperationEfficiency (std::stod (valor));
        } else if (clave == "initialEnergy")
        {
          consumptionModel->SetInitialEnergy (std::stod (valor));
        } else
        {
          NS_LOG_ERROR ("Could not parse XML file. Key " << clave << " unrecognized.");
        }
      }
    }

    
    //all consumption model has a mobility model
    Ptr<MobilityModel> mobility = node->GetObject<MobilityModel> ();
    consumptionModel->SetMobilityModel (mobility);

    //we add the consumption model to the node
    node->AggregateObject (consumptionModel);

    Simulator::Schedule (Seconds (m_updateTime), &UpdateModelConsumption, consumptionModel, m_updateTime);
  }

  Ptr<Node> 
  GetNodeFromContext (std::string context)
  {
    std::string delimiter = "/";

    size_t i = 0;
    size_t count = 0;
    std::string token;
    while ((i = context.find(delimiter)) != std::string::npos) {
        count++;
        token = context.substr(0, i);
        context.erase(0, i + delimiter.length());
        if (count == 3)
        {
          return NodeList::GetNode (std::stoi(token));
        }
    }
    return NULL;
  }

  std::string
  Convert(const xmlChar * xmlChar)
  {
    char* a = (char *)xmlChar;
    return std::string(a);
  }

} // namespace ns3