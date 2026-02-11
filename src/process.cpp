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

int Process::Pid() { return pid_; }

float Process::CpuUtilization() const {
  long total_time = LinuxParser::ActiveJiffies(pid_);
  long seconds = LinuxParser::UpTime(pid_);

  if (seconds > 0)
    return (static_cast<float>(total_time) / sysconf(_SC_CLK_TCK)) / seconds;

  return 0.0;
}

string Process::Command() { return LinuxParser::Command(pid_); }

string Process::Ram() { return LinuxParser::Ram(pid_); }

string Process::User() { return LinuxParser::User(pid_); }

long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

bool Process::operator<(const Process &a) const {
  return this->CpuUtilization() < a.CpuUtilization();
}
