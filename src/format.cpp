#include "format.h"

#include <iomanip>
#include <sstream>
#include <string>

using std::string;

string Format::ElapsedTime(long seconds) {
  const long hours = seconds / 3600L;
  const long minutes = (seconds % 3600L) / 60L;
  const long _seconds = (seconds % 60L);

  std::ostringstream ostringstream;
  ostringstream << std::setfill('0') << std::setw(2) << hours << ":"
  << std::setw(2) << minutes << ":" << std::setw(2) << _seconds;
  return ostringstream.str();
}
