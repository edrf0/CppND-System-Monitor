#include "linux_parser.h"

#include <unistd.h>

#include <filesystem>
#include <sstream>
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
  string os, kernel, version;
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
// Updated to the C++ way
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  std::filesystem::path path(kProcDirectory);

  for (const auto &entry : std::filesystem::directory_iterator(path)) {
    if (entry.is_directory()) {
      string filename(entry.path().filename().string());
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }

  return pids;
}

float LinuxParser::MemoryUtilization() {
  std::ifstream fstream(kProcDirectory + kMeminfoFilename);
  float memTotal = 0.0, memFree = 0.0, buffers = 0.0, cached = 0.0;

  if (fstream.is_open()) {
    std::string line;
    while (std::getline(fstream, line)) {
      std::istringstream linestream(line);
      std::string key, value;
      linestream >> key >> value;
      if (key == "MemTotal:") {
        memTotal = stof(value);
      } else if (key == "MemFree:") {
        memFree = stof(value);
      } else if (key == "Buffers:") {
        buffers = stof(value);
      } else if (key == "Cached:") {
        cached = stof(value);
      }
      // This stops further scanning of the file
      // because all values will be set by then
      if (memTotal > 0.0 && memFree > 0.0 && buffers > 0.0 && cached > 0.0) {
        break;
      }
    }
  }

  // Prevents division by zero if fstream cannot open the file
  if (memTotal == 0.0) return 0.0;

  float usedMemory = memTotal - memFree - buffers - cached;
  return usedMemory / memTotal;
}

long LinuxParser::UpTime() {
  std::ifstream fstream(kProcDirectory + kUptimeFilename);
  long uptime = 0;

  if (fstream.is_open()) fstream >> uptime;

  return uptime;
}

long LinuxParser::Jiffies() {
  auto values = CpuUtilization();
  if (values.empty()) return 0;

  long totalJiffies = 0;
  for (const string& value:values) {
    totalJiffies += stol(value);
  }
  return totalJiffies;
}

long LinuxParser::ActiveJiffies(int pid) {
  string line, value;
  vector<string> values;
  long totalJiffiesForPID = 0;
  std::ifstream fstream(kProcDirectory + std::to_string(pid) + kStatFilename);

  if (fstream.is_open()) {
    std::getline(fstream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      values.push_back(value);
    }
    if (values.size() > 17) {
      for (int i = 13; i < 17; ++i) {
        totalJiffiesForPID += stol(values[i]);
      }
    }
  }
  return totalJiffiesForPID;
}

long LinuxParser::ActiveJiffies() {
  auto jiffies = CpuUtilization();

  return Jiffies() - IdleJiffies();
}

long LinuxParser::IdleJiffies() {
  auto values = CpuUtilization();

  if (values.size() > kIOwait_) {
    long idle = std::stol(values[kIdle_]);
    long iowait = std::stol(values[kIOwait_]);

    return idle + iowait;
  }

  return 0;
}

vector<string> LinuxParser::CpuUtilization() {
  std::ifstream fstream(kProcDirectory + kStatFilename);
  vector<string> cpuValues;

  if (fstream.is_open()) {
    std::string line,key,value;
    std::getline(fstream, line);
    std::istringstream linestream(line);
    // This step captures the cpu text so we can loop next over the values
    linestream >> key;
    while (linestream >> value) {
      cpuValues.push_back(value);
    }
  }
  return cpuValues;
}

int LinuxParser::TotalProcesses() {
  std::ifstream fstream(kProcDirectory + kStatFilename);
  int value = 0;
  string line,key;

  if (fstream.is_open()) {
    while (std::getline(fstream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") {
        return value;
      }
    }
  }
  return value;
}

int LinuxParser::RunningProcesses() {
  std::ifstream fstream(kProcDirectory + kStatFilename);
  int value = 0;
  string line,key;

  if (fstream.is_open()) {
    while (std::getline(fstream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running") {
        return value;
      }
    }
  }
  return value;
}

string LinuxParser::Command(int pid) {
  std::ifstream ifstream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  string command;
  if (ifstream.is_open()) std::getline(ifstream, command);

  return command;
}

string LinuxParser::Ram(int pid) {
  string line, key;
  long value;
  std::ifstream ifstream(kProcDirectory + std::to_string(pid) + kStatusFilename);

  if (ifstream.is_open()) {
    while (std::getline(ifstream,line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmRSS:") {
        return std::to_string(value / 1024);
      }
    }
  }
  return "0";
}

string LinuxParser::Uid(int pid) {
  string line, key, value;
  std::ifstream ifstream(kProcDirectory + std::to_string(pid) + kStatusFilename);

  if (ifstream.is_open()) {
    while (std::getline(ifstream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid:") {
        return value;
      }
    }
  }

  return "";
}

string LinuxParser::User(int pid) {
  string uid = Uid(pid);
  string line,user,x,target_uid;

  std::ifstream ifstream(kPasswordPath);
  if (ifstream.is_open()) {
    while (std::getline(ifstream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> x >> target_uid;
      if (target_uid == uid) {
        return user;
      }
    }
  }

  return user;
}

long LinuxParser::UpTime(int pid) {
  string line, value;
  long start_time = 0;

  std::ifstream ifstream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (ifstream.is_open()) {
    std::getline(ifstream, line);
    std::istringstream linestream(line);

    for (int i = 1; i <= 22; ++i) {
      linestream >> value;
      if (i == 22) {
        start_time = stol(value);
      }
    }
  }
  // Converting clock ticks to seconds
  long start_time_seconds = start_time / sysconf(_SC_CLK_TCK);
  // Process uptime
  return UpTime() - start_time_seconds;
}
