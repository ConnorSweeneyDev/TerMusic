#include "utility.hpp"

#include <algorithm>
#include <cctype>
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

  bool is_number(const std::string &string)
  {
    return !string.empty() && std::find_if(string.begin(), string.end(), [](unsigned char character)
                                           { return !std::isdigit(character); }) == string.end();
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
