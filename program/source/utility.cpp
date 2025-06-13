#include "utility.hpp"

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

namespace tuim::utility
{
  std::string seconds_to_string(const double &current_seconds)
  {
    int minutes = static_cast<int>(round(current_seconds)) / 60;
    int seconds = static_cast<int>(round(current_seconds)) % 60;
    return (minutes < 10 ? "0" : "") + std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") +
           std::to_string(seconds);
  }

  void log(const std::string &message)
  {
    std::ofstream log_file("log.txt");
    if (!log_file.is_open())
    {
      std::cerr << "Failed to open log file!" << std::endl;
      exit(EXIT_FAILURE);
    }
    log_file << message << std::endl;
    log_file.close();
  }
}
