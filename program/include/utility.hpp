#pragma once

#include <string>

namespace tuim::utility
{
  std::string seconds_to_string(const double &current_seconds);
  bool is_number(const std::string &string);
  void log(const std::string &message);
}
