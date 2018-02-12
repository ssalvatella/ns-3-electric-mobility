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

#ifndef ELECTRIC_MOBILITY_HELPER_H
#define ELECTRIC_MOBILITY_HELPER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "ns3/ns2-mobility-helper.h"

namespace ns3 {

class ElectricMobilityHelper 
{
public:
  /**
   * \param filename filename of file which contains the
   *        xml with vehicles attributes
   */
  ElectricMobilityHelper (std::string filename, Ns2MobilityHelper ns2, std::ofstream *os);

  /**
   * Read the xml file and configure the movement
   * patterns of all nodes contained in the global ns3::NodeList
   * whose nodeId is matches the nodeId of the nodes in the trace
   * file.
   */
  void Install (void) const;

  std::string m_filename; // filename of file containing the vehicle attributes
  Ns2MobilityHelper m_ns2; // ns2 mobility helper for access the mobility of nodes
  std::ofstream m_os; // log file output
};

  uint32_t GetNodeIdFromContext(std::string context);

} // namespace ns3

#endif /* ELECTRIC_MOBILITY_HELPER_H */