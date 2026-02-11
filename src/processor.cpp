#include "processor.h"

#include "linux_parser.h"

float Processor::Utilization() {
  long total_time = LinuxParser::Jiffies();
  long idle_time = LinuxParser::IdleJiffies();

  long delta_total = total_time - prev_total_time;
  long delta_idle = idle_time - prev_idle_time;

  // Updating for next call
  prev_total_time = total_time;
  prev_idle_time = idle_time;

  return static_cast<float>(delta_total - delta_idle) / delta_total;
}
