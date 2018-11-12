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

#ifndef ELECTRIC_CONSUMPTION_HELPER_H
#define ELECTRIC_CONSUMPTION_HELPER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include "ns3/electric-vehicle-consumption-model.h"

namespace ns3 {

class ElectricConsumptionHelper 
{
public:
  /**
   * \param filename filename of file which contains the
   *        xml with vehicles attributes
   */
  ElectricConsumptionHelper (std::string filename, double updateTime);

  /**
   * Read the xml file and configure the movement
   * patterns of all nodes contained in the global ns3::NodeList
   * whose nodeId is matches the nodeId of the nodes in the trace
   * file.
   */
  void Install (void);


private:
  void LoadXml (void);
  void CreateModelFromXml (xmlNode * xmlNode);

private:
  std::string m_filename;  // filename of file containing the vehicle attributes
  xmlDoc *m_xmlDoc;        // XML Document with XML nodes
  double m_updateTime;     // time between each update of electric vehicle consumption
};

  Ptr<Node> GetNodeFromContext(std::string context);
  std::string Convert(const xmlChar * xmlChar);


} // namespace ns3

#endif /* ELECTRIC_CONSUMPTION_HELPER_H */