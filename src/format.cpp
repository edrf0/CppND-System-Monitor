#include "format.h"

#include <sstream>
#include <string>

using std::string;

string Format::ElapsedTime(long seconds) {
  int hours = seconds / 3600;
  int minutes = (seconds % 3600) / 60;
  int _seconds = (seconds % 60);

  std::ostringstream ostringstream;
  ostringstream << hours << "h:" << minutes << "m:" << _seconds;
  return ostringstream.str();
}
