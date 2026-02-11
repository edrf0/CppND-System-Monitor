#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();

 private:
  long prev_total_time = 0;
  long prev_idle_time = 0;
};

#endif