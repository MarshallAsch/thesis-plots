/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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
 */

/* This script configures two nodes connected vis CSMA channel. One of the
 *  nodes transmits packets via PPBP application to the other node.
 *  Command line parameters are the simulationTime and verbose for logging.
 *  Example run: Copy to scratch folder and run
 *  ./waf --run "scratch/PPBP-application-test --simulationTime=10.0 --verbose=true"
 *  Author: Sharan Naribole <nsharan@rice.edu>
 */

/**
 *
 * This is an inital base test of a NS3 simulation that will create a one goup of nodes
 * and have them move arround randomly. This will also create an animation for netanim
 *
 * This is the first part in the progressive tesing.
 * the next step, part 2 will attempt to setup a wireless mesh network for the nodes.
 *
 *
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"

#include "ns3/mobility-module.h"
#include "ns3/netanim-module.h"

#include "simulation-params.h"

using namespace speedFalloff;
using namespace ns3;

ns3::Time ticTime;

NS_LOG_COMPONENT_DEFINE("NS3 learning test");

void tick(NodeContainer c) {
  double current = Simulator::Now().GetNanoSeconds();

  for (NodeContainer::Iterator n = c.Begin(); n != c.End(); n++) {
    Ptr<Node> node = *n;
    uint32_t id = node->GetId();

    Ptr<const MobilityModel> m = node->GetObject<MobilityModel>();
    Vector pos = m->GetPosition();
    Vector vel = m->GetVelocity();

    std::cout << "< time=\"" << current << "\" node=\"" << id << "\" locX=\"" << pos.x
              << "\" locY=\"" << pos.y << "\" velX=\"" << vel.x << "\" velY=\"" << vel.y << "\" >"
              << std::endl;
  }

  Simulator::Schedule(ticTime, &tick, c);
}

/**
 * This is the main entry point to start the simulator.
 * Needs to set everything up so that it can run
 *
 */
int main(int argc, char* argv[]) {
  RngSeedManager::SetSeed(7);  // arbitrarily chosen
  Time::SetResolution(Time::NS);

  /* Setup and parse the command line options. */
  // Set up the command line options.
  SimulationParameters params;
  bool ok;
  std::tie(params, ok) = SimulationParameters::parse(argc, argv);

  if (!ok) {
    std::cerr << "Error parsing the parameters.\n";
    return -1;
  }

  // create node containers
  NodeContainer nodes;
  nodes.Create(params.totalNodes);

  // create mobility model
  MobilityHelper mobility;
  mobility.SetPositionAllocator(params.area.getRandomRectanglePositionAllocator());

  if (params.mobilityModel == SimulationParameters::Model::WALK) {
    mobility.SetMobilityModel(
        "ns3::RandomWalk2dMobilityModel",
        "Bounds",
        RectangleValue(params.area.asRectangle()),
        "Speed",
        PointerValue(params.velocity),
        "Time",
        TimeValue(params.changeTime),
        "Distance",
        DoubleValue(params.changeDistance),
        "Mode",
        EnumValue(params.walkMode));
  } else if (params.mobilityModel == SimulationParameters::Model::WAYPOINT) {
    mobility.SetMobilityModel(
        "ns3::RandomWaypointMobilityModel",
        "Speed",
        PointerValue(params.velocity),
        "Pause",
        PointerValue(params.pauseTime),
        "PositionAllocator",
        PointerValue(params.area.getRandomRectanglePositionAllocator()));
  }

  mobility.Install(nodes);

  ticTime = params.ticTime;

  Simulator::Schedule(Seconds(0.0), &tick, nodes);

  Simulator::Stop(params.runtime);
  Simulator::Run();
  Simulator::Destroy();

  return 0;
}
