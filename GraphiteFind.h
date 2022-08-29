#pragma once

#include <string>
#include "GraphiteFinder.h"

class GraphiteFind {
 public:
  GraphiteFind(std::string & q){
    query = q;
    result = Find(query, 0, 0);
  }
 private:
  std::string query;
  Result result;
};


