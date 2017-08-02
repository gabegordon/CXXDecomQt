#ifdef __MINGW64__
#include <mingwthread.hpp>
#else
#include <thread>
#endif
#include <boost/algorithm/string.hpp>
#include "pdsDecode.hpp"
#include "backend.hpp"
#include "getFiles.hpp"
#include "LogFile.hpp"
#include "ReadFile.hpp"
#include "ProgressBar.hpp"
#include "Decom.hpp"

/**
 * Returns list of choices for decom. Passed to Qt for user selection.
 *
 * @param directory Directory to look in for files.
 * @return Set containing the file options.
 */
std::set<std::string> pdsDecode::getFileTypeNames(const std::string& directory)
{
    m_directory = directory;
    std::set<std::string> outfileNames;
    m_files = getFiles::filesInDirectory(m_directory, ".PDS");
    if (m_files.size() == 0)
    {
        LogFile::logError("No .pds files found");
        exit(0);
    }

    for (const auto& filename : m_files)
    {
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
            std::string outfilename = getFileName(filename);
            outfileNames.emplace(outfilename);
            infile.close();
        }
    }
    return outfileNames;
}

/**
 * Main PDSDecode/Decom function. Read PDS files contents into vectors, which are then passed by queue to a thread resposible for running the Decom class.
 *
 * @param backend Pointer to Qt backend for callbacks and status updates.
 * @param selectedFiles List of user selected files, used for skipping unselected files.
 * @param debug Debug flag.
 * @param entries Database entries to be passed to the Decom class.
 * @param type Satellite type.
 * @param bigEndian Endian format of input data.
 */
void pdsDecode::init(BackEnd* backend, const std::vector<std::string>& selectedFiles, const bool& debug, const std::vector<DataTypes::Entry>& entries, const DataTypes::SCType& type, const bool& bigEndian)
{
    Decom decomEngine(debug, entries, type, bigEndian);
    std::thread decomThread(&Decom::init, decomEngine, &m_queue, backend);
    ProgressBar pbar(m_files.size(), "Parsing PDS");
    uint32_t i = 0;
    for (const auto& filename : m_files)
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
    backend->setProgress("Waiting for writer threads to finish.");
    m_queue.setInactive();  // Notify Decom that no more h5 files will be pushed onto the queue
}

/**
 * Gets filename from full file path string.
 *
 * @param filename Full file path.
 * @return Filename string.
 */
std::string pdsDecode::getFileName(const std::string& filename)
{
    std::vector<std::string> splitstring;
    boost::split(splitstring, filename, boost::is_any_of("/"));  // Split file path delimeters

    std::string name_without_path = splitstring.back();  // Name will be at the end of the path
    return name_without_path.substr(0, 8);  // First 8 characters is description without date
}
