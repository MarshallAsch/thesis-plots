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

using namespace ns3;
bool verbose = true;

NS_LOG_COMPONENT_DEFINE("NS3 learning test");

void tick(NodeContainer c) {
  for (NodeContainer::Iterator n = c.Begin(); n != c.End(); n++) {
    Ptr<Node> node = *n;
    uint32_t id = node->GetId();

    Ptr<const MobilityModel> m = node->GetObject<MobilityModel>();
    Vector pos = m->GetPosition();
    Vector vel = m->GetVelocity();

    std::cout << "< time=\"" << Simulator::Now().GetNanoSeconds() << "\" node=\"" << id
              << "\" locX=\"" << pos.x << "\" locY=\"" << pos.y << "\" velX=\"" << vel.x
              << "\" velY=\"" << vel.y << "\" >" << std::endl;
  }

  Simulator::Schedule(Seconds(1.0), &tick, c);
}

/**
 * This is the main entry point to start the simulator.
 * Needs to set everything up so that it can run
 *
 */
int main(int argc, char* argv[]) {
  double simulationTime = 3600 * 4;  // seconds

  int numNodes = 500;  // convert pop dencity to this num for 5k x 5k area
  double max_y = 1000;
  double max_x = 1000;

  double min_speed = 0;
  double max_speed = 30;
  double min_pause = 0;
  double max_pause = 0;

  Time::SetResolution(Time::NS);

  // this will set a seed so that the same numbers are not generated each time.
  // the run number should be incremented each time this simulation is run to ensure streams do not
  // overlap
  RngSeedManager::SetSeed(3);
  RngSeedManager::SetRun(1);

  // start loop here for number of runs

  // create node containers
  NodeContainer nodes;
  nodes.Create(numNodes);

  // create position allocator
  Ptr<RandomRectanglePositionAllocator> positionAlloc =
      CreateObject<RandomRectanglePositionAllocator>();

  Ptr<RandomVariableStream> x = CreateObject<UniformRandomVariable>();
  Ptr<RandomVariableStream> y = CreateObject<UniformRandomVariable>();
  Ptr<RandomVariableStream> speed = CreateObject<UniformRandomVariable>();
  Ptr<RandomVariableStream> pause = CreateObject<UniformRandomVariable>();

  speed->SetAttribute("Min", DoubleValue(min_speed));
  speed->SetAttribute("Max", DoubleValue(max_speed));

  pause->SetAttribute("Min", DoubleValue(min_pause));
  pause->SetAttribute("Max", DoubleValue(max_pause));

  x->SetAttribute("Min", DoubleValue(0.0));
  x->SetAttribute("Max", DoubleValue(max_x));

  y->SetAttribute("Min", DoubleValue(0.0));
  y->SetAttribute("Max", DoubleValue(max_y));

  positionAlloc->SetX(x);
  positionAlloc->SetY(y);
  positionAlloc->SetZ(0.0);  // all of the nodes are at ground level, do I need to change this?

  // create mobility model
  MobilityHelper mobility;

  mobility.SetMobilityModel(
      "ns3::RandomWaypointMobilityModel",
      "Speed",
      PointerValue(speed),
      "Pause",
      PointerValue(pause),
      "PositionAllocator",
      PointerValue(positionAlloc));
  mobility.SetPositionAllocator(PointerValue(positionAlloc));

  mobility.Install(nodes);

  Simulator::Schedule(Seconds(0.0), &tick, nodes);

  Simulator::Stop(Seconds(simulationTime));
  Simulator::Run();
  Simulator::Destroy();

  return 0;
}
