#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/tee.hpp>
#include <iostream>

#include "LogFile.h"


typedef boost::iostreams::tee_device<std::ostream, std::ofstream> Tee;
typedef boost::iostreams::stream<Tee> TeeStream;

/**
 * Helper function to log errors to both stderr and to file
 *
 * @param message Message to output
 * @return N/A
 */
void LogFile::logError(const std::string& message)
{
    std::ofstream file( "Error_Log.txt" );
    Tee tee(std::cerr, file);
    TeeStream both(tee);
    both << message << std::endl;
    file.close();
}
