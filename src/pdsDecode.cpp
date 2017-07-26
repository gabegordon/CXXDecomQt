#include <boost/algorithm/string.hpp>
#include "pdsDecode.h"
#include "backend.h"
#include "getFiles.h"
#include "LogFile.h"
#include "ReadFile.h"
#include "ProgressBar.h"
/**
 * Main PDS decode function. Reads all PDS's in folder and writes packet files.
 *
 * @return set of files written
 */
std::set<std::string> pdsDecode::init(BackEnd* backend)
{
    std::set<std::string> outfileNames;
    std::string test = "1;";
    std::vector<std::string> files = getFiles::filesInFolder(m_directory, "/*.PDS");
    if(files.size() == 0)
    {
        LogFile::logError("No .pds files found");
        exit(0);
    }

    ProgressBar pbar(files.size(), "Parsing PDS");
    uint32_t i = 0;
    for (const auto& filename : files)
    {
        pbar.Progressed(i++, backend);
        std::ifstream infile(filename, std::ios::binary);
        uint8_t firstByte = 0;
        ReadFiles::read(firstByte, infile);

        if(firstByte == 0)  // If firstbyte is zeroed then it is a header file that we can ignore.
        {
            continue;
            infile.close();
        }
        else
        {
            infile.seekg(0, std::ios::beg);  // Go back to start of the file
            std::string outfilename = getFileName(filename);
            std::ofstream& outfile = getStream(outfilename);
            outfile << infile.rdbuf();  // Write contents to outfile

            outfileNames.emplace(outfilename);
            infile.close();
        }
    }
    for(auto& ofile : m_outfiles)
        ofile.second.close();
    return outfileNames;
}

/**
 * Gets correct output file and returns stream.
 *
 * @param outfilename Filename
 * @return ofstream reference
 */
std::ofstream& pdsDecode::getStream(const std::string& outfilename)
{
    auto& ofile = m_outfiles[outfilename];
    if (!ofile.is_open())
    {
        ofile.open("output/" + outfilename, std::ios::binary);
    }
    return ofile;
}

std::string pdsDecode::getFileName(const std::string& filename)
{
    std::vector<std::string> splitstring;
    boost::split(splitstring, filename, boost::is_any_of("/"));  // Split file path delimeters

    std::string name_without_path = splitstring.back();  // Name will be at the end of the path
    return name_without_path.substr(0, 8);  // First 8 characters is description without date
}
