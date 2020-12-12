//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0
#ifndef TRAIN_LOADER_H_
#define TRAIN_LOADER_H_

// #include <bits/stdint-uintn.h>
#include <string>
#include <unordered_map>
#include <vector>
#include "models.h"

// Global namespace.
struct JourneyLeg {
  int trainLine;
  int src;
  int dest;
  int journeyLegTime;
};

struct TrainJourney {
  std::vector<JourneyLeg> legs;
  int journey_minutes;
};

class TrainLoader
{
public:
  TrainLoader(const std::string& overlapFile, const std::string& journeyFile,
  const std::string & routeFile);
  ~TrainLoader() {} // Empty dtor.

  int GetOverlapMinutes(int src1, int dest1, int src2, int dest2) const;
  int GetJourneyMinutes(int src, int dest) const;
  std::vector<JourneyLeg> GetJourneyLegs(int src, int dest) const;

  std::vector<int> GetTrainLineStations(int trainLine, bool isDown) const;
  bool GetJourneyLegStationsAndIsDown(int trainLine, int src, int dest, std::vector<int>& stations) const;
  int GetOverlapMinutesAlongLine(int line, int src1, int dest1, int src2, int dest2) const;


private:
  std::unordered_map<count_type, int> overlap_times;
  std::unordered_map<count_type, TrainJourney> journeys;
  std::unordered_map<int, std::vector<int>> train_lines;
};

#endif // TRAIN_LOADER_H_
