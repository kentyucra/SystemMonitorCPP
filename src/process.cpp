#include "process.h"

#include <unistd.h>

#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// TODO: Return this process's ID
int Process::Pid() { return pid; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const {
  return LinuxParser::ActiveJiffies(pid);
}

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid); }

// TODO: Return this process's memory utilization
string Process::Ram() {
  string memoSize = LinuxParser::Ram(pid);
  return std::to_string(std::stoi(memoSize) / 1000);
}

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() {
  vector<string> stats = LinuxParser::extractStats(pid);
  if (stats.size() < 22) return 0;
  float starttime = 0;
  if (std::all_of(stats[21].begin(), stats[21].end(), isdigit))
    starttime = std::stol(stats[21]);
  return LinuxParser::UpTime() - (starttime / sysconf(_SC_CLK_TCK));
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a [[maybe_unused]]) const {
  return CpuUtilization() > a.CpuUtilization();
}