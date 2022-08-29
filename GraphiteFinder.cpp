

#include "GraphiteFinder.h"
#include "Where.h"
#include "fmt/format.h"
#include <regex>
#include <chrono>

int IndexDirect = 1;
int IndexAuto = 2;
int IndexReversed = 3;
int ReverseLevelOffset = 10000;
int TreeLevelOffset = 20000;
int ReverseTreeLevelOffset = 30000;
std::string DefaultTreeDate = "1970-02-12";
int PrefixNotMatched = 1;
int PrefixMatched = 2;
int PrefixPartialMathed = 3;

std::unordered_map<std::string, size_t> index_reverse =  {
    {"direct", IndexDirect},
    {"auto", IndexAuto},
    {"reversed", IndexReversed},
};
std::shared_ptr<GraphiteFinder> new_plain_finder(const std::string &query, int from, int until) {
  Options ops{};
  std::string url = "url";
  std::string table = "table";
  return std::make_shared<IndexFinder>(url, table, ops, true, 1, true, 1);
}
Result Find(const std::string &query, int from, int until) {
  std::shared_ptr<GraphiteFinder> f = new_plain_finder(query, from, until);
  std::string q = f->generate_query(query, from, until);
  std::cout << q;
  return Result{};
}

Where BaseFinder::where(const std::string &query) {
  std::string::difference_type level = std::count(query.begin(), query.end(), '.') + 1;
  Where w;
  w.And(eq("Level", level));
  w.And(TreeGlob("Path", query));
  return w;
}

std::string BaseFinder::generate_query(const std::string &query, int from, int until) {
  Where w = where(query);
  std::string q = fmt::format("SELECT Path FROM {} WHERE {} GROUP BY Path FORMAT TabSeparatedRaw", table, w.string());
  return q;
}

std::string BlackListFinder::generate_query(const std::string &query, int from, int until) {
  for (const std::regex& re : blacklist) {
    std::smatch m;
    if (std::regex_match(query, m, re)){
      matched = true;
      return "";
    }
  }
  return wrapped->generate_query(query, from, until);
}
std::string DateFinder::generate_query(const std::string &query, int from, int until) {
  Where w = finder.where(query);
  Where date_where;
  char time_from[100];
  char time_until[100];
  struct tm t_from= {.tm_sec = from, .tm_year = 1020};
  struct tm t_until= {.tm_sec = until , .tm_year = 1020};
  strftime(time_from, 50, "%Y-%m-%e", &t_from);
  strftime(time_until, 50, "%Y-%m-%e", &t_until);

  date_where.And(fmt::format("Date >='{}' AND Date <= '{}'",
                             time_from, time_until));

  std::string q = fmt::format("SELECT Path FROM {} WHERE ({}) AND ({}) GROUP BY Path FORMAT TabSeparatedRaw", finder.table, date_where.string(), w.string());
  return q;

}
std::string DateFinderV3::generate_query(const std::string &query, int from, int until) {

  Where w = finder.where(reverse_string(query));
  Where date_where;
  char time_from[100];
  char time_until[100];
  struct tm t_from= {.tm_sec = from, .tm_year = 1020};
  struct tm t_until= {.tm_sec = until , .tm_year = 1020};
  strftime(time_from, 50, "%Y-%m-%e", &t_from);
  strftime(time_until, 50, "%Y-%m-%e", &t_until);

  date_where.And(fmt::format("Date >='{}' AND Date <= '{}'",
                             time_from, time_until));

  std::string q = fmt::format("SELECT Path FROM {} WHERE ({}) AND ({}) GROUP BY Path FORMAT TabSeparatedRaw", finder.table, date_where.string(), w.string());
  return q;

}
Where IndexFinder::where(const std::string &query, int levelOffset) {
  std::string::difference_type level = std::count(query.begin(), query.end(), '.') + 1;
  Where w;
  w.And(eq("Level", level+levelOffset));
  w.And(TreeGlob("Path", query));
  return w;
}
uint8_t IndexFinder::check_reverses(const std::string &query) {
  for (const auto& rule : conf_reverses) {
    if (!rule.prefix.empty() && !str_has_prefix(query, rule.prefix)) {
      continue;
    }
    if (!rule.suffix.empty() && !str_has_suffix(query, rule.suffix)) {
      continue;
    }
    std::smatch m;
    if (!std::regex_match(query, m, rule.regex)){
      continue;
    }
    return index_reverse[rule.reverse];
  }
  return conf_reverse;
}
bool IndexFinder::use_reverse(const std::string &query) {
  if (reverse == IndexDirect) {
    return false;
  } else if (reverse == IndexReversed) {
    return true;
  }

  reverse = check_reverses(query);
  if (reverse != IndexAuto) {
    return use_reverse(query);
  }

  size_t w = index_wildcard(query);
  if (w == std::string::npos) {
    reverse = IndexDirect;
    return use_reverse(query);
  }
  std::string before_wildcard = query.substr(0, w);

  std::string::difference_type firstWildcardNode = std::count(before_wildcard.begin(), before_wildcard.end(), '.') + 1;

  w = index_last_wildcard(query);
  std::string after_wildcard = query.substr(w);
  std::string::difference_type lastWildcardNode = std::count(before_wildcard.begin(), before_wildcard.end(), '.') + 1;
  if (firstWildcardNode < lastWildcardNode) {
    reverse = IndexReversed;
    return use_reverse(query);
  }
  reverse = IndexDirect;
  return use_reverse(query);
}

std::string IndexFinder::generate_query(const std::string &query, int from, int until) {


  std::string new_query = query;
  if (daily_enabled && from > 0 && until > 0) {
    use_daily = true;
  } else {
    use_daily = false;
  }

  int levelOffset = 0;
  if (use_daily) {
    if (use_reverse(new_query)) {
      levelOffset = ReverseLevelOffset;
    }
  } else {
    if (use_reverse(new_query)) {
      levelOffset = ReverseTreeLevelOffset;
    } else {
      levelOffset = TreeLevelOffset;
    }
  }

  if (use_reverse(new_query)) {
    new_query = reverse_string(new_query);
  }

  Where w = where(new_query, levelOffset);


  char time_from[100];
  char time_until[100];
  struct tm t_from= {.tm_sec = from, .tm_year = 1020};
  struct tm t_until= {.tm_sec = until , .tm_year = 1020};
  strftime(time_from, 50, "%F", &t_from);
  strftime(time_until, 50, "%F", &t_until);

  w.And(fmt::format("Date >='{}' AND Date <= '{}'",
                             time_from, time_until));



  if (use_daily) {
    w.And(fmt::format("Date >='{}' AND Date <= '{}'",
                      time_from, time_until));
  } else {
    w.And(eq(("Date"), DefaultTreeDate));
  }

  std::string q = fmt::format("SELECT Path FROM {} WHERE {} GROUP BY Path FORMAT TabSeparatedRaw", table, w.string());
  return q;
}

std::string PrefixFinder::generate_query(const std::string &query, int from, int until) {
  std::vector<std::string> qs = split(query, ".");
  std::vector<std::string> ps = split(prefix, ".");
  for (int i = 0; i < qs.size() && i < ps.size(); i++) {
    std::smatch m;
    if (!std::regex_search(ps[i], m, std::regex("^"+glob_to_regexp(qs[i])+"$"))) {
      return "";
    }
  }
  if (qs.size() < ps.size()) {

    part = join(qs, 0, qs.size(), ".") + ".";
    matched = PrefixMatched;
  }
  return wrapped->generate_query(join(qs, ps.size(), qs.size(), "."), from, until);

}
std::string ReverseFinder::generate_query(const std::string &query, int from, int until) {
  std::string::difference_type p = std::count(query.begin(), query.end(), '.') + 1;
  if (p < 0 || p >= query.size() - 1) {
    return wrapped->generate_query(query, from, until);
  }

  std::string new_query = query.substr(p+1);
  if (has_wildcard(new_query)) {
    wrapped->generate_query(query, from, until);
  }
  is_used = true;
  return base_finder.generate_query(reverse_string(query), from, until);

}

std::string GraphiteTagQ::string() const {
  if (param != nullptr && value != nullptr) {
    return "{" + fmt::format(R"("param"={}, "value"={})", *param, *value) + "}";
  }
  if (param != nullptr) {
    return "{" + fmt::format("\"param\"={}", *param) + "}";
  }
  if (value != nullptr) {
    return "{" + fmt::format("\"value\"={}", *value) + "}";
  }
  return "{}";
}

std::string GraphiteTagQ::where(const std::string &field) const {
  if (param != nullptr && value != nullptr && *value != "*") {
    return eq(field, *param + *value);
  }
  if (param != nullptr) {
    return has_prefix(field, *param);
  }
  if (value != nullptr && *value != "*") {
    return eq(field, *value);
  }
  return "{}";
}
