#pragma once
#include <fstream>

class LogFile
{
  public:

    static void logError(const std::string& message);

  private:
    // Disallow creating an instance of this object
    LogFile() {}
};
