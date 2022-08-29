#pragma once

#include <string>
#include "GraphiteFinder.h"
#include "GraphiteFind.h"


class GraphiteFindHandler {
  void handleRequest(){
    GraphiteFind f(query);
  }
  void Reply(){

  }
 private:
  std::string query;
  std::string format;
};
