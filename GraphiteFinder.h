#pragma once

#include <string>
#include <regex>
#include <unordered_map>
#include "Where.h"
#include <iostream>


class Result{
 public:
  std::string GetList();
  std::vector<std::string> GetSeries();
  std::string GetAbs(std::string & v);
};

struct IndexReverseRule {
 std::string suffix;
 std::string prefix;
 std::string regex_str;
 std::regex regex;
 std::string reverse;
};


extern int IndexDirect;
extern int IndexAuto;
extern int IndexReversed;

extern int ReverseLevelOffset;
extern int TreeLevelOffset;
extern int ReverseTreeLevelOffset;
extern std::string DefaultTreeDate;

extern int PrefixNotMatched;
extern int PrefixMatched;
extern int PrefixPartialMathed;


extern std::unordered_map<std::string, size_t> index_reverse;

struct Options{
  int64_t timeout;
  int64_t connect_timeout;
};



class GraphiteTagQ {
 public:
  std::string *param;
  std::string *value;

  std::string string() const;

  std::string where(const std::string &field) const;
};



class GraphiteFinder {
 public:
  //  std::vector<std::string> list();
//  std::vector<std::string> series();
//  std::vector<std::string> abs(std::vector<std::string>);
  virtual std::string generate_query(const std::string &query, int from, int until) {
    return "error generating string";
  };
  virtual Result GetResult(){
    return Result{};
  };
  virtual ~GraphiteFinder()= default;
  virtual std::string Abs(std::string & v){
    return "error abs";
  };
};

class BaseFinder: virtual public GraphiteFinder{
 public:
  BaseFinder(std::string &url, std::string &table, Options opts): url(url), table(table), opts(opts){}
  std::string generate_query(const std::string &query, int from, int until) override;
  Where where(const std::string &query);

//  std::vector<std::string> list();
//  std::vector<std::string> series();
//  std::vector<std::string> abs(std::string v);
  std::string url;
  std::string table; //graphite tree table
  Options opts;
  Result result;
  ~BaseFinder() override = default;

 private:

//  std::vector<std::string> make_list(std::string only_series);

};

class BlackListFinder: public GraphiteFinder{
 public:
  BlackListFinder(std::shared_ptr<GraphiteFinder> &wrapped, std::vector<std::regex> &blacklist): wrapped(wrapped), blacklist(blacklist), matched(false){}
  std::string generate_query(const std::string &query, int from, int until) override;
  Result GetResult() override{
    return result;
  }
  Result result;
 private:
  std::shared_ptr<GraphiteFinder> wrapped;
  std::vector<std::regex> blacklist;
  bool matched;
};

class DateFinder: public GraphiteFinder{
 public:
  DateFinder(std::string &url, std::string &table, Options opts, int tableVersion): finder(url, table, opts), tableVersion(tableVersion){}
  std::string generate_query(const std::string &query, int from, int until) override;
  Result GetResult() override{
    return result;
  }
  BaseFinder finder;

 private:
  Result result;
  int tableVersion;
};

class DateFinderV3: public GraphiteFinder{
 public:
  DateFinderV3(std::string &url, std::string &table, Options opts): finder(url, table, opts){}
  std::string generate_query(const std::string &query, int from, int until) override;
  Result GetResult() override{
    return result;
  }
  BaseFinder finder;
  Result result;
};

class IndexFinder : public GraphiteFinder{
 public:
  IndexFinder(std::string &url, std::string &table, Options opts,  bool daily_enabled, uint8_t conf_reverse, bool use_daily,  uint8_t reverse)
  :url(url),table(table), opts(opts), daily_enabled(daily_enabled), conf_reverse(conf_reverse), conf_reverses(1),reverse(), use_daily(use_daily) {};
  Where where(const std::string &query, int levelOffset);
  uint8_t check_reverses(const std::string &query);
  bool use_reverse(const std::string &query);
  std::string generate_query(const std::string &query, int from, int until) override;
  Result GetResult() override{
    return result;
  }

  std::string url;
  std::string table; //graphite tree table
  Options opts;
  bool daily_enabled;
  size_t conf_reverse;
  std::vector<IndexReverseRule> conf_reverses;
  uint8_t reverse;
  std::string body;
  bool use_daily;
  Result result;
};

class MockFinder: public GraphiteFinder{ //TODO mock
  std::vector<char *> result;
  std::string query;
};

class PrefixFinder: public GraphiteFinder{
 public:
  PrefixFinder(std::shared_ptr<GraphiteFinder> &wrapped, std::string &prefix, char *prefixBytes, int matched, std::string &part):
  wrapped(wrapped), prefix(prefix), matched(matched), part(part){
    strcat(prefixBytes, ".");
    prefix_bytes = prefixBytes;
  }
  std::string generate_query(const std::string &query, int from, int until) override;
  Result GetResult() override{
    return result;
  }

  std::shared_ptr<GraphiteFinder> wrapped;
  std::string prefix;
  char * prefix_bytes;
  int matched;
  Result result;
  std::string &part;
};

class ReverseFinder: public GraphiteFinder{
 public:
  ReverseFinder(std::shared_ptr<GraphiteFinder> &wrapped, BaseFinder &base_finder, std::string &url, std::string &table, bool is_used, Options opts)
  :
  wrapped(wrapped), base_finder(url, table, opts), url(url), table(table), is_used(is_used), opts(opts){}
  std::string generate_query(const std::string &query, int from, int until) override;
  Result GetResult() override{
    return result;
  }

  std::shared_ptr<GraphiteFinder> wrapped;
  BaseFinder base_finder;
  std::string &url;
  std::string &table;
  bool is_used;
  Options opts;
  Result result;
};

class TagFinder : public GraphiteFinder{
 public:
  TagFinder(std::shared_ptr<GraphiteFinder> &wrapped,std::string &url,std::string &table,Options opts,int state)
  :
  wrapped(wrapped), url(url), table(table), opts(opts), state(state){};
  std::shared_ptr<GraphiteFinder> wrapped;
  std::string &url;
  std::string &table;
  Options opts;
  int state;
  std::vector<GraphiteTagQ> taq_query;
  std::string  series_query;
  char * tag_prefix{};
  char * body{};
  Result result;

};

class TaggedFinder : public GraphiteFinder {
  std::shared_ptr<GraphiteFinder> wrapped;
  std::string url;
  std::string table;
  Options opts;
  // tagged_costs
  char * body;
};

std::shared_ptr<GraphiteFinder> new_plain_finder(const std::string &query, int from, int until);

Result Find(const std::string &query, int from, int until);
