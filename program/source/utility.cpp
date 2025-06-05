#include "utility.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

namespace tuim::utility
{
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
