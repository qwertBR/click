
#include "ConverTime.h"
int IntervalStrings(std::string &s, int sign) {
  if (s[0] == '-'){
    sign = -1;
    s = s.substr(1);
  } else if (s[1] == '+'){
    sign = 1;
    s = s.substr(1);
  }
  int totalInterval = 0;
  while(!s.empty()){
    int j = 0;
    while(j < s.size() && std::isdigit(s[j])){
      j++;
    }
    std::string offsetStr;
    offsetStr = s.substr(0, j);
    s = s.substr(j);
    j = 0;
    while(j < s.size() && std::isalpha(s[j])){
      j++;
    }
    std::string unitStr;
    unitStr = s.substr(0, j);
    s = s.substr(j);
    int units = 0;
    if (unitStr == "s" || unitStr == "sec" || unitStr == "second" || unitStr == "seconds") {
      units = 1;
    } else if (unitStr == "m" || unitStr == "min" || unitStr == "mins" || unitStr == "minute" || unitStr == "minutes"){
      units = 60;
    } else if (unitStr == "h" || unitStr == "hour" || unitStr == "hours") {
      units = 60 * 60;
    } else if (unitStr == "d" || unitStr == "day" || unitStr == "days"){
      units = 24 * 60 * 60;
    } else if (unitStr == "w" || unitStr == "week" || unitStr == "weeks"){
      units = 7 * 24 * 60 * 60;
    } else if (unitStr == "mon" || unitStr == "month" || unitStr == "months"){
      units = 30 * 24 * 60 * 60;
    } else if (unitStr == "y" || unitStr == "year" || unitStr == "years"){
      units = 365 * 24 * 60 * 60;
    };
    int offset = stoi(offsetStr);
    totalInterval += sign * offset * units;
  }
  return totalInterval;
}
