
#pragma once

#include <vector>
#include <string>
#include <map>
#include <map>
#include <unordered_map>
#include <string>
#include "GraphiteFormatter.h"
#include "GraphiteFinder.h"




void find(std::string &query, int from, int until) {

}




/// Response with custom string. Can be used for browser.
class GraphiteRenderHandler
{
 private:

  int status;
  std::string content_type;
  std::string response_expression;

 public:
  GraphiteRenderHandler();

  void writeResponse(std::string & out);

  void handleRequest(std::string & request, std::string & response){
    std::shared_ptr<GraphiteFormatter> formatter = get_formatter(request);
    MultiTarget fetchRequests = formatter->ParseRequest(request);
    Map am;
    std::size_t metricsLen = 0;
    for (const auto& el : fetchRequests){
      for (const auto& expr : el.second.list){
        auto fndResult = Find(expr, el.first.From, el.first.Until);
        merge_target(am, fndResult, expr);
        metricsLen += am.size();
      }
    }


  }
};


