#include "format.h"

#include <iomanip>
#include <iostream>
#include <string>

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  int ss = seconds % 60;
  int mm = seconds / 60;
  int hh = mm / 60;
  mm = mm % 60;
  std::stringstream stst;
  stst << std::setfill('0') << std::setw(2) << hh << ':' << std::setw(2) << mm
       << ':' << std::setw(2) << ss;
  return stst.str();
}