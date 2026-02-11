#include "linux_parser.h"

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
      std::stringstream linestream(line);
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

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  std::ifstream stream(kProcDirectory + kUptimeFilename);

  return 0;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { return {}; }

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { return 0; }

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { return 0; }

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }
