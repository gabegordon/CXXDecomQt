#pragma once
#include <fstream>
#include <string>
#include <sstream>
#include "LogFile.hpp"

namespace ReadFiles
{
template <typename B, typename F>
    /**
     * Templated read function. Reads Type T from stream.
     *
     * @param buffer Data type to read into
     * @param in Stream to read from
     */
    static inline void read(B& buffer, F& in)
    {
        in.read(reinterpret_cast<char *>(&buffer), sizeof(buffer));
    }

    /**
     * Checks if a filestream is valid and open.
     * Prints error message and exits if invalid.
     *
     * @param stream Stream to check
     * @param name Name to print out for debugging purposes
     */
    static inline void checkFile(std::ifstream& stream, const std::string& name)
    {
        if (!stream || !stream.is_open())
        {
            LogFile::logError("Failed to open " + name + " file.");
            exit(0);
        }
    }
}
