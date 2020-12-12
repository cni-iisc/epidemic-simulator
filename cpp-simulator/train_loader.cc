//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0

#include "train_loader.h"
// #include <bits/stdint-uintn.h>
#include <algorithm>
#include <iostream>
#include <rapidjson/document.h>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

namespace {
  std::regex leg_regex("(\\d+), (\\d+), (\\d+), (\\d+)");

  auto readJSONFile(const std::string& filename){
  std::ifstream ifs(filename, std::ifstream::in);
  rapidjson::IStreamWrapper isw(ifs);
  rapidjson::Document d;
  d.ParseStream(isw);
  return d;
  }

  std::vector<JourneyLeg> RouteStringToJourneyLeg(std::string routeStr) {
    std::smatch m;
    std::vector<JourneyLeg> legs;
    while(std::regex_search(routeStr, m, leg_regex)) {
      JourneyLeg leg;
      leg.trainLine = std::stoi(m[1]);
      leg.src = std::stoi(m[2]);
      leg.dest = std::stoi(m[3]);
      leg.dest = std::stoi(m[3]);
      leg.journeyLegTime = std::stoi(m[4]);
      legs.push_back(leg);
      routeStr = m.suffix();
    }
    return legs;
  }
}

TrainLoader::TrainLoader(const std::string& overlapFile,
  const std::string& journeyFile,
  const std::string & routeFile) {
  if(GLOBAL.ENABLE_COHORTS){
    std::cout<<"\nLOADING TRAIN STATIC DATA...";

    // load travel time.
    std::cout<<"starting to read station JSONs...";
    auto travelJSON = readJSONFile(journeyFile);
    journeys.reserve(travelJSON.GetArray().Size());

    // Sample: {"start":55,"end":49,"travel_time_min":18}
    for (auto &elem: travelJSON.GetArray()){

      TrainJourney train_journey;
      int start = elem["start"].GetInt();
      int end = elem["end"].GetInt();
      // uint64_t key = 100*start + end;
      count_type key = 100*start + end;
      train_journey.journey_minutes = elem["travel_time_min"].GetInt();
      train_journey.legs = RouteStringToJourneyLeg(elem["route_str"].GetString());
      journeys[key] = train_journey;
    }

    std::cout << "travel time loaded...";

    // load overlap time.
    auto routeJSON = readJSONFile(routeFile);
    std::vector<int> v;
    train_lines[1] = v;
    train_lines[2] = v;
    train_lines[3] = v;
    train_lines[4] = v;
    // Sample: {"trainLineId":3,"stationId":47,"slowTimeFromPreviousStation":3,
    // "latitude":19.016,"longitude":72.8588,"wardIndex":47,
    // "stationName":"vadala road","stationCode":"VD"}
    for (auto &elem: routeJSON.GetArray()){
      int line = elem["trainLineId"].GetInt();
      int station = elem["stationId"].GetInt();
      train_lines[line].push_back(station);
    }

    std::cout << " routes loaded...";

    // load overlap time.
    auto overlapJSON = readJSONFile(overlapFile);
    overlap_times.reserve(overlapJSON.GetArray().Size());
    // Sample: {"src1":0,"dest1":14,"src2":46,"dest2":21,"overlap_time_min":8}
    for (auto &elem: overlapJSON.GetArray()){
      int src1 = elem["src1"].GetInt();
      int dest1 = elem["dest1"].GetInt();
      int src2 = elem["src2"].GetInt();
      int dest2 = elem["dest2"].GetInt();
      int overlap_time_min = elem["overlap_time_min"].GetInt();
      // uint64_t key = ((src1*100 + dest1)*100 + src2)*100 + dest2;
      count_type key = ((src1*100 + dest1)*100 + src2)*100 + dest2;
      overlap_times[key] = overlap_time_min;
    }
    std::cout << " overlap time loaded...DONE" << std::endl;
  }
}

int TrainLoader::GetOverlapMinutes(
  int src1, int dest1, int src2, int dest2) const {
  // keeping key human readable.
  // uint64_t key = ((src1*100 + dest1)*100 + src2)*100 + dest2;
  count_type key = ((src1*100 + dest1)*100 + src2)*100 + dest2;
  auto citer = overlap_times.find(key);
  if (citer == overlap_times.end()) {
    std::cout<<"\n\n\n INVALID INPUT FOR TRAIN OVERLAP"
        << " src1="  << src1
        << " dest1=" << dest1
        << " src2="  << src2
        << " dest2=" << dest2
        << " key="   << key
        << std::endl;
    return -1; // Invalid input.
  }

  return citer->second;
}

/* int TrainLoader::GetJourneyMinutes(int src, int dest) const {
  // keeping key human readable.
  uint64_t key = src*100 + dest;
  auto citer = journeys.find(key);
  if (citer == journeys.end()) {
    std::cout<<"\n\n\n INVALID INPUT FOR TRAIN OVERLAP"
        << " src="  << src
        << " dest=" << dest
        << " key="   << key
        << std::endl;
    return -1; // Invalid input.
  }
  return citer->second.journey_minutes;
} */

int TrainLoader::GetJourneyMinutes(int src, int dest) const {
  auto journey_legs = GetJourneyLegs( src, dest);
  int journey_time = 0;
  for(auto& journey_leg: journey_legs){
    journey_time+=journey_leg.journeyLegTime;
  }
  return journey_time;
}


std::vector<JourneyLeg> TrainLoader::GetJourneyLegs(int src, int dest) const {
  // keeping key human readable.
// uint64_t key = src*100 + dest;
  count_type key = src*100 + dest;
  auto citer = journeys.find(key);
  if (citer == journeys.end()) {
    std::cout<<"\n\n\n INVALID INPUT FOR TRAIN OVERLAP"
        << " src="  << src
        << " dest=" << dest
        << " key="   << key
        << std::endl;
    std::vector<JourneyLeg> legs;
    return legs; // Invalid input.
  }
  return citer->second.legs;
}


std::vector<int> TrainLoader::GetTrainLineStations(
  int trainLine, bool isDown) const {

  auto citer = train_lines.find(trainLine);
  if (citer == train_lines.end()) {
    std::vector<int> v;
    return v;
  }
  if (isDown) return citer->second;

  auto v = citer->second;
  std::reverse(v.begin(), v.end());
  return v;
}

bool TrainLoader::GetJourneyLegStationsAndIsDown(
  int trainLine, int src, int dest, std::vector<int>& v) const {
  auto train_line = GetTrainLineStations(trainLine, true);

  int start_index = -1;
  int end_index = -1;
  for (size_t i =0; i < train_line.size(); i++) {
    if (train_line[i] != src && train_line[i] != dest) {
      continue;
    }
    if (start_index < 0) {
      start_index = i;
    } else if(end_index < 0) {
      end_index = i;
      break;
    }
  }

  if (start_index < 0 || end_index < 0) {
    return false;
  }

  for (int i = start_index; i <= end_index; i++) {
    v.push_back(train_line[i]);
  }
  if (v[0] == src) {
    return true;
  }
  std::reverse(v.begin(), v.end());
  return false;
 }


int TrainLoader::GetOverlapMinutesAlongLine(
  int line, int src1, int dest1, int src2, int dest2) const
{
  auto journey1 = GetJourneyLegs(src1, dest1);
  auto journey2 = GetJourneyLegs(src2, dest2);
  int legIndex1 = -1;
  int legIndex2 = -1;
  for (size_t index =0; index < journey1.size(); index++) {
    if (journey1[index].trainLine == line) {
      legIndex1 = index;
      break;
    }
  }
  for (size_t index =0; index < journey2.size(); index++) {
    if (journey2[index].trainLine == line) {
      legIndex2 = index;
      break;
    }
  }
  if(legIndex1<0 || legIndex2<0) {
    return 0;
  }

  return GetOverlapMinutes(journey1[legIndex1].src, journey1[legIndex1].dest,
                           journey2[legIndex2].src, journey2[legIndex2].dest);
}
