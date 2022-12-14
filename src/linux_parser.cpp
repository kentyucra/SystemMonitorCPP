#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string name;
  int value;
  string measure;
  float MemTotal;
  float MemFree;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> name >> value >> measure;
      if (name == "MemTotal") {
        MemTotal = value;
      } else if (name == "MemFree") {
        MemFree = value;
      }
    }
  }
  return MemFree / MemTotal;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  float systemUpTime;
  string line;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> systemUpTime;
  }
  return (long)systemUpTime;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  string line = "";
  string val = "";
  float utime = 0, stime = 0, cutime = 0, cstime = 0, starttime = 0;

  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  if (filestream.is_open()) {
    if (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      int index = 0;
      while (linestream >> val && filestream.is_open()) {
        if (index == 13 && std::all_of(val.begin(), val.end(), isdigit))
          utime = std::stol(val);
        if (index == 14 && std::all_of(val.begin(), val.end(), isdigit))
          stime = std::stol(val);
        if (index == 15 && std::all_of(val.begin(), val.end(), isdigit))
          cutime = std::stol(val);
        if (index == 16 && std::all_of(val.begin(), val.end(), isdigit))
          cstime = std::stol(val);
        if (index == 21 && std::all_of(val.begin(), val.end(), isdigit))
          starttime = std::stol(val);
        index++;
      }
    }
  }
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

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return ActiveJiffies() + IdleJiffies(); }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  string line;
  string name;
  int user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> name;
      if (name == "cpu") {
        linestream >> user >> nice >> system >> idle >> iowait >> irq >>
            softirq >> steal >> guest >> guest_nice;
      }
    }
  }
  return user + nice + system + irq + softirq + steal + guest + guest_nice;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  string line;
  string name;
  int user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> name;
      if (name == "cpu") {
        linestream >> user >> nice >> system >> idle >> iowait >> irq >>
            softirq >> steal >> guest >> guest_nice;
      }
    }
  }
  return idle + iowait;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { return {}; }

vector<string> LinuxParser::extractStats(int pid) {
  vector<string> stats;
  string line = "";
  string val = "";

  try {
    std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                             kStatFilename);
    if (filestream.is_open()) {
      if (std::getline(filestream, line)) {
        std::istringstream linestream(line);
        while (linestream >> val && filestream.is_open()) {
          stats.push_back(val);
        }
      }
    }
  } catch (...) {
    stats = vector<string>();
  }
  return stats;
}

int ExtractValueFromStats(string nameSearched) {
  string line;
  string name;
  int value;
  int valueSearched;
  std::ifstream filestream(LinuxParser::kProcDirectory +
                           LinuxParser::kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> name >> value;
      if (name == nameSearched) {
        valueSearched = value;
      }
    }
  }

  return valueSearched;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { return ExtractValueFromStats("processes"); }

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  return ExtractValueFromStats("procs_running");
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid [[maybe_unused]]) {
  string line;
  string val;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    return line;
  }
  return line;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid [[maybe_unused]]) {
  string line;
  string name;
  string memoSize = "0";
  string path = kProcDirectory + std::to_string(pid) + kStatusFilename;
  std::ifstream filestream(path);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> name;
      if (name == "VmSize:") {
        // std::cout << "entraasdasdasdjkllkasjd!!!";
        linestream >> memoSize;
      }
    }
  }
  // std::cout << "memoSize = " << memoSize << std::endl;
  return memoSize;
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid [[maybe_unused]]) {
  string line;
  string name;
  string uid;
  string path = kProcDirectory + std::to_string(pid) + kStatusFilename;
  // std::cout << "path = " << path << std::endl;
  std::ifstream filestream(path);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> name;
      if (name == "Uid:") {
        // std::cout << "entraasdasdasdjkllkasjd!!!";
        linestream >> uid;
      }
    }
  }
  return uid;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid [[maybe_unused]]) {
  string userID = LinuxParser::Uid(pid);
  string line;
  string user;
  string uid;
  string x;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), ':', ' ');

      std::istringstream linestream(line);
      linestream >> user >> x >> uid;

      if (uid == userID) {
        return user;
      }
    }
  }

  return string();
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid [[maybe_unused]]) { return 0; }
