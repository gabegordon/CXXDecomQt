#include <boost/algorithm/string.hpp>
#include "pdsDecode.h"
#include "backend.h"
#include "getFiles.h"
#include "LogFile.h"
#include "ReadFile.h"
#include "ProgressBar.h"
#include "Decom.h"

std::set<std::string> pdsDecode::getFileTypeNames(const std::string& directory)
{
    m_directory = directory;
    std::set<std::string> outfileNames;
    std::vector<std::string> files = getFiles::filesInDirectory(m_directory, ".PDS");
    if(files.size() == 0)
    {
        LogFile::logError("No .pds files found");
        exit(0);
    }

    for (const auto& filename : files)
    {
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
            std::string outfilename = getFileName(filename);
            outfileNames.emplace(outfilename);
            infile.close();
        }
    }
    return outfileNames;
}

/**
 * Main PDS decode function. Reads all PDS's in folder and writes packet files.
 *
 * @return set of files written
 */
void pdsDecode::init(BackEnd* backend, const std::vector<std::string>& selectedFiles, const bool& debug, const std::vector<DataTypes::Entry>& entries, const bool& NPP)
{
    std::vector<std::string> files = getFiles::filesInDirectory(m_directory, ".PDS");
    if(files.size() == 0)
    {
        LogFile::logError("No .pds files found");
        exit(0);
    }

    Decom decomEngine(debug, entries, NPP);
    std::thread decomThread(&Decom::init, decomEngine, std::ref(m_queue));
    ProgressBar pbar(files.size(), "Parsing PDS");
    uint32_t i = 0;
    for (const auto& filename : files)
    {
        backend->setCurrentFile(filename);
        pbar.Progressed(++i, backend);
        std::ifstream infile(filename, std::ios::binary);
        uint8_t firstByte = 0;
        ReadFiles::read(firstByte, infile);

        if (firstByte == 0)  // If firstbyte is zeroed then it is a header file that we can ignore.
        {
            continue;
            infile.close();
        }
        else
        {
            if (std::find(std::begin(selectedFiles), std::end(selectedFiles), getFileName(filename)) != std::end(selectedFiles))
            {

                infile.seekg(0, std::ios::beg);  // Go back to start of the file
                std::vector<uint8_t> contents((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
                m_queue.push(std::make_tuple(contents, "CERES"));
            }
            infile.close();
        }
    }
}

/**
 * Gets filename from full file path string.
 *
 * @param filename Full file path
 * @return Filename string
 */
std::string pdsDecode::getFileName(const std::string& filename)
{
    std::vector<std::string> splitstring;
    boost::split(splitstring, filename, boost::is_any_of("/"));  // Split file path delimeters

    std::string name_without_path = splitstring.back();  // Name will be at the end of the path
    return name_without_path.substr(0, 8);  // First 8 characters is description without date
}
