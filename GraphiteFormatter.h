#pragma once

#include <vector>
#include <string>
#include <map>
#include <map>
#include <unordered_map>
#include <string>
#include <memory>
#include <cstddef>
#include "GraphiteFinder.h"


struct Value {
  Value(std::string & Target,  std::string & DisplayName): Target(Target), DisplayName(DisplayName){};
  std::string Target;
  std::string DisplayName;
};

using Map = std::unordered_map<std::string, std::vector<std::shared_ptr<Value>>>;

struct Data{

};

void merge_target(Map &m, Result & r, const std::string & target){
//  auto series = r.GetSeries();
//  for (auto key : series) {
//    if (key.empty()) {
//      continue;
//    }
//    auto abs = r.GetAbs(key);
//    m[key].push_back(std::make_shared<Value>(target, abs));
//  }
}

struct Targets {
  std::vector<std::string> list;
  std::unordered_map<std::string, Value> AM;
  std::string pointsTable;
  bool isReverse = false;
};

struct TimeFrame {
  int From;
  int Until;
  int MaxDataPoints;
};

struct CHResponse  {
  Data  *Data;
  int From;
  int Until;
};

struct TimeFrameHash {
  std::size_t operator()(const TimeFrame& k) const
  {
    return std::hash<int>()(k.From) ^
        (std::hash<int>()(k.Until) << 1);
  }
};

struct TimeFrameEqual {
  bool operator()(const TimeFrame& lhs, const TimeFrame& rhs) const
  {
    return lhs.From == rhs.From && lhs.From == rhs.Until && lhs.MaxDataPoints == rhs.MaxDataPoints;
  }
};

using MultiTarget = std::unordered_map<TimeFrame, Targets, TimeFrameHash, TimeFrameEqual>;

using DataMap = std::unordered_map<std::string, Value>;





class GraphiteFormatter {
 public:
  virtual MultiTarget ParseRequest(std::string & r);;
  virtual void Reply(){};
  virtual ~GraphiteFormatter() = default;
};

class PickleFormat : public GraphiteFormatter{
 public:
  MultiTarget ParseRequest(std::string & r) override;
  void Reply() override;
};

class CarbonApiV3 : public GraphiteFormatter{
 public:
  MultiTarget ParseRequest(std::string & r) override;
  void Reply() override;
};

class CarbonApiV2 : public GraphiteFormatter{
 public:
  CarbonApiV2(std::string & r) : request(r){};
  MultiTarget ParseRequest(std::string & r) override;
  void Reply() override {};
  ~CarbonApiV2() override = default;
 private:
  std::string request;
};

std::shared_ptr<GraphiteFormatter> get_formatter(std::string & request){
  return std::make_shared<CarbonApiV2>(request);
}


