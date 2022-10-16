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
  vector<string> stats = LinuxParser::extractStats(pid);
  if (stats.size() < 22) return 0;
  float utime = 0, stime = 0, cutime = 0, cstime = 0, starttime = 0;
  if (std::all_of(stats[13].begin(), stats[13].end(), isdigit))
    utime = std::stol(stats[13]);
  if (std::all_of(stats[14].begin(), stats[14].end(), isdigit))
    stime = std::stol(stats[14]);
  if (std::all_of(stats[15].begin(), stats[15].end(), isdigit))
    cutime = std::stol(stats[15]);
  if (std::all_of(stats[16].begin(), stats[16].end(), isdigit))
    cstime = std::stol(stats[16]);
  if (std::all_of(stats[21].begin(), stats[21].end(), isdigit))
    starttime = std::stol(stats[21]);
  float uptime = LinuxParser::UpTime();

  float totalTime = utime + stime + cutime + cstime;
  float seconds = uptime - (starttime / sysconf(_SC_CLK_TCK));
  float m_utilization = 0;
  try {
    m_utilization = ((totalTime / sysconf(_SC_CLK_TCK)) / seconds);

  } catch (...) {
    m_utilization = 0;
  }

  return m_utilization;
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