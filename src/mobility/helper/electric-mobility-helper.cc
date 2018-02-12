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

#include "ns3/core-module.h"
#include "ns3/node-list.h"
#include "ns3/node.h"
#include "ns3/log.h"
#include "ns3/mobility-module.h"
#include "ns3/mobility-module.h"
#include "ns3/ns2-mobility-helper.h"
#include "electric-mobility-helper.h"

namespace ns3 {

  NS_LOG_COMPONENT_DEFINE ("ElectricMobilityHelper");

  ElectricMobilityHelper::ElectricMobilityHelper (std::string filename, Ns2MobilityHelper ns2, std::ofstream *os)
    : m_filename (filename), m_ns2 (ns2)
  {
    std::ifstream file (m_filename.c_str (), std::ios::in);
    if (!(file.is_open ())) NS_FATAL_ERROR("Could not open vehicule attributes file " << m_filename.c_str() << " for reading, aborting here \n");
  }

  void 
  ElectricMobilityHelper::Install (void) const 
  {

  }

  uint32_t 
  GetNodeIdFromContext (std::string context)
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
          return std::stoi(token);
        }
    }
    return -1;
  }



} // namespace ns3