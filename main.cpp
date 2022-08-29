#include <iostream>
#include <string>
#include "Where.h"
#include "ConverTime.h"
#include <chrono>
#include "GraphiteFinder.h"




#include <iostream>
#include <iomanip>
#include <ctime>
#include <fmt/format.h>



using namespace std;

int main() {
  auto f = new_plain_finder("", 1, 1);
  cout << f->generate_query("host.top.cpu.cpu*2A", 11, 1543473323);

}