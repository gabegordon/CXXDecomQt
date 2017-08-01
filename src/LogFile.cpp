#include <iostream>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/tee.hpp>
#include "LogFile.hpp"

typedef boost::iostreams::tee_device<std::ostream, std::ofstream> Tee;
typedef boost::iostreams::stream<Tee> TeeStream;

/**
 * Helper function to log errors to both stderr and to file.
 *
 * @param message Message to output.
 */
void LogFile::logError(const std::string& message)
{
    std::ofstream logfile("output/Error_Log.txt", std::ios::app);
    Tee tee(std::cerr, logfile);
    TeeStream both(tee);
    both << message << std::endl;
    logfile.close();
}
