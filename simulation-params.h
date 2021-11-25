/// \file simulation-params.h
/// \author Keefer Rourke <mail@krourke.org>
/// \brief All simulation parameters are declared in this file.
///
/// Copyright (c) 2020 by Keefer Rourke <krourke@uoguelph.ca>
/// Permission to use, copy, modify, and/or distribute this software for any
/// purpose with or without fee is hereby granted, provided that the above
/// copyright notice and this permission notice appear in all copies.
///
/// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
/// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
/// AND FITNESS. IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
/// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
/// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
/// OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
/// PERFORMANCE OF THIS SOFTWARE.

#ifndef __SPEED_FALLOFF_SIMULATION_PARAMS_H
#define __SPEED_FALLOFF_SIMULATION_PARAMS_H

#include <inttypes.h>
#include <utility>

#include "ns3/nstime.h"
#include "ns3/random-variable-stream.h"
#include "ns3/random-walk-2d-mobility-model.h"

#include "simulation-area.h"

namespace speedFalloff {

/// \brief Encapsulates parsing parameters from the CommandLine.
///     Each public member of this class corresponds to a strongly typed object
///     built from one or more of the commandline arguments.
///     Default parameter values are defined in the SimulationParameters::parse
///     method.
/// \see SimulationParameters::parse
class SimulationParameters {
 public:
  enum class Model { WALK = 0, WAYPOINT };

  /// Simulation runtime.
  ns3::Time runtime;
  /// Total nodes used in the simulation.
  uint32_t totalNodes;
  /// The simulation area.
  SimulationArea area;
  /// The velocity of the nodes, used for both random walk and waypoint
  ns3::Ptr<ns3::UniformRandomVariable> velocity;
  /// The pause time of the nodes, used for random waypoint
  ns3::Ptr<ns3::UniformRandomVariable> pauseTime;
  /// The period after which nodes should change their direction if
  /// the walkMode is MODE_TIME.
  ns3::Time changeTime;
  // the amount of time to wait between printing the node information
  ns3::Time ticTime;

  /// The distance after which nodes should change their direction if
  /// the walkMode is MODE_DISTANCE.
  double changeDistance;
  /// Governs the behaviour of the traveller nodes' walking.
  ns3::RandomWalk2dMobilityModel::Mode walkMode;

  // if the wandom waypoint or random walk mobility model should be used
  Model mobilityModel;

  SimulationParameters() {}

  /// \brief Parses command line options to set simulation parameters.
  ///
  /// \param argc The number of command line options passed to the program.
  /// \param argv The raw program arguments.
  /// \return std::pair<SimulationParameters, bool>
  ///   If the boolean value is false, there was an error calling the program,
  ///   and so the construction of the simulation parameters does not make sense.
  ///   If it is true, construction succeeded and the simulation may run.
  ///
  static std::pair<SimulationParameters, bool> parse(int argc, char* argv[]);
};

}  // namespace speedFalloff

#endif
