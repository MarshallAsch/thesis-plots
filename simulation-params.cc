/// \file simulation-params.cc
/// \author Keefer Rourke <krourke@uoguelph.ca>
/// \author Marshall Asch <masch@uoguelph.ca>
///
/// Copyright (c) 2020 by Keefer Rourke <krourke@uoguelph.ca>
/// Copyright (c) 2021 by Marshall Asch <masch@uoguelph.ca>
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

#include <inttypes.h>
#include <algorithm>
#include <cctype>
#include <string>
#include <utility>

#include "ns3/command-line.h"
#include "ns3/double.h"
#include "ns3/nstime.h"
#include "ns3/random-variable-stream.h"
#include "ns3/random-walk-2d-mobility-model.h"

#include "simulation-params.h"

namespace speedFalloff {
using namespace ns3;

/// User defined literal for metric lengths.
constexpr int32_t operator"" _meters(const unsigned long long meters) {
  return static_cast<int32_t>(meters);
}
constexpr double operator"" _meters(const long double meters) { return meters; }

/// User defined literal for unit measures in seconds.
constexpr double operator"" _seconds(const long double seconds) { return seconds; }

/// User defined literal for minute measures, auto converted to seconds.
constexpr double operator"" _minutes(const long double minutes) { return minutes * 60; }

/// User defined literal for hours measures, auto converted to seconds.
constexpr double operator"" _hours(const long double hours) { return hours * 60 * 60; }

/// User defined literal for meters/second velocities.
constexpr double operator"" _mps(const long double mps) { return mps; }

std::pair<ns3::RandomWalk2dMobilityModel::Mode, bool> getWalkMode(std::string str) {
  std::pair<ns3::RandomWalk2dMobilityModel::Mode, bool> result;
  result.second = false;
  std::string lower = str;
  std::transform(str.begin(), str.end(), lower.begin(), ::tolower);
  if (lower == "distance") {
    result.first = ns3::RandomWalk2dMobilityModel::Mode::MODE_DISTANCE;
    result.second = true;
  } else if (lower == "time") {
    result.first = ns3::RandomWalk2dMobilityModel::Mode::MODE_TIME;
    result.second = true;
  }
  return result;
}

std::pair<SimulationParameters::Model, bool> getMobilityModel(std::string str) {
  std::pair<SimulationParameters::Model, bool> result;
  result.second = false;
  std::string lower = str;
  std::transform(str.begin(), str.end(), lower.begin(), ::tolower);
  if (lower == "walk") {
    result.first = SimulationParameters::Model::WALK;
    result.second = true;
  } else if (lower == "waypoint") {
    result.first = SimulationParameters::Model::WAYPOINT;
    result.second = true;
  }
  return result;
}

// static
std::pair<SimulationParameters, bool> SimulationParameters::parse(int argc, char* argv[]) {
  /* Default simulation values. */
  // Simulation run time.
  double optRuntime = 4.0_hours;
  double optTicTime = 1.0_seconds;

  // Node parameters.
  uint32_t optTotalNodes = 1;

  // Simulation area parameters.
  double optAreaWidth = 1000.0_meters;
  double optAreaLength = 1000.0_meters;

  double optMinSpeed = 0.0_mps;
  double optMaxSpeed = 30.0_mps;
  double optMinPauseTime = 0.0_seconds;
  double optMaxPauseTime = 0.0_seconds;

  double optChangeTime = 0.0_seconds;
  double optChangeDistance = 0.0_meters;

  std::string optWalkMode = "distance";
  std::string optMobilityModel = "waypoint";

  /* Setup commandline option for each simulation parameter. */
  CommandLine cmd;
  cmd.AddValue("runTime", "Simulation run time in seconds", optRuntime);
  cmd.AddValue("ticTime", "The time between printing node information in seconds", optTicTime);
  cmd.AddValue("totalNodes", "Total number of nodes in the simulation", optTotalNodes);
  cmd.AddValue("areaWidth", "Width of the simulation area in meters", optAreaWidth);
  cmd.AddValue("areaLength", "Length of the simulation area in meters", optAreaLength);
  cmd.AddValue(
      "mobilityModel",
      "Which mobility model should be used random walk or random waypoint"
      "passed; options are 'walk' or 'waypoint' ",
      optMobilityModel);
  // random waypoint mobility model options
  cmd.AddValue("minSpeed", "Minimum velocity of nodes in m/s", optMinSpeed);
  cmd.AddValue("maxSpeed", "Maximum velocity of nodes in m/s", optMaxSpeed);
  cmd.AddValue(
      "minPauseTime",
      "Minimum amount of time to pause for before moving to the next waypoint in s",
      optMinPauseTime);
  cmd.AddValue(
      "maxPauseTime",
      "Maximum amount of time to pause for before moving to the next waypoint in s",
      optMaxPauseTime);
  // random walk mobility model options
  cmd.AddValue(
      "changeTime",
      "The amount of time to wait before changing dirrections in s",
      optChangeTime);
  cmd.AddValue(
      "changeDistance",
      "The amount of distance for the node to move before changing direction in m",
      optChangeDistance);
  cmd.AddValue(
      "walkMode",
      "Should the random walk mobility model change direction after distance walked or time "
      "passed; options are 'distance' or 'time' ",
      optWalkMode);
  cmd.Parse(argc, argv);

  /* Parse the parameters. */

  bool ok = true;
  SimulationParameters result;

  RandomWalk2dMobilityModel::Mode walkMode;
  std::tie(walkMode, ok) = getWalkMode(optWalkMode);
  if (!ok) {
    std::cerr << "Unrecognized walk mode '" + optWalkMode + "'." << std::endl;
  }

  SimulationParameters::Model mobilityModel;
  std::tie(mobilityModel, ok) = getMobilityModel(optMobilityModel);
  if (!ok) {
    std::cerr << "Unrecognized mobility model '" + optWalkMode + "'." << std::endl;
  }

  Ptr<UniformRandomVariable> velocityGenerator = CreateObject<UniformRandomVariable>();
  velocityGenerator->SetAttribute("Min", DoubleValue(optMinSpeed));
  velocityGenerator->SetAttribute("Max", DoubleValue(optMaxSpeed));

  Ptr<UniformRandomVariable> pauseTimeGenerator = CreateObject<UniformRandomVariable>();
  pauseTimeGenerator->SetAttribute("Min", DoubleValue(optMinPauseTime));
  pauseTimeGenerator->SetAttribute("Max", DoubleValue(optMaxPauseTime));

  result.ticTime = Seconds(optTicTime);
  result.runtime = Seconds(optRuntime);
  result.area = SimulationArea(
      std::pair<double, double>(0.0, 0.0),
      std::pair<double, double>(optAreaWidth, optAreaLength));

  result.totalNodes = optTotalNodes;
  result.mobilityModel = mobilityModel;

  result.velocity = velocityGenerator;
  result.pauseTime = pauseTimeGenerator;

  result.changeDistance = optChangeDistance;
  result.changeTime = Seconds(optChangeTime);
  result.walkMode = walkMode;

  return std::pair<SimulationParameters, bool>(result, ok);
}
}  // namespace speedFalloff
