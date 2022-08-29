
#include "GraphiteFormatter.h"
MultiTarget CarbonApiV2::ParseRequest(std::string &r) {
  MultiTarget mul;
  TimeFrame tf = {.From=12, .Until=14, .MaxDataPoints=5};
  std::vector<std::string> targets;
  std::unordered_map<std::string, Value> am;
  Targets tar = {.list = targets, .AM = am};
  mul[tf] = tar;
  return mul;
}
MultiTarget GraphiteFormatter::ParseRequest(std::string &r) {
  MultiTarget mul;
  TimeFrame tf = {.From=12, .Until=14, .MaxDataPoints=5};
  std::vector<std::string> targets;
  std::unordered_map<std::string, Value> am;
  Targets tar = {.list = targets, .AM = am};
  mul[tf] = tar;
  return mul;
}
