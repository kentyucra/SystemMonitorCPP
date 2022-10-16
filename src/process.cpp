#include "process.h"

#include <unistd.h>

#include <cctype>
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
  float utime = std::stoi(stats[13]);
  float stime = std::stoi(stats[14]);
  float cutime = std::stoi(stats[15]);
  float cstime = std::stoi(stats[16]);
  float starttime = std::stoi(stats[21]);
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

  float starttime = std::stoi(stats[21]);
  return LinuxParser::UpTime() - (starttime / sysconf(_SC_CLK_TCK));
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a [[maybe_unused]]) const {
  return CpuUtilization() > a.CpuUtilization();
}