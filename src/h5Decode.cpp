#include <iostream>
#include <cstdint>
#include <set>
#include <algorithm>
#include <iterator>
#ifdef __MINGW64__
#include <mingwthread.hpp>
#else
#include <thread>
#endif
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include "h5Decode.hpp"
#include "getFiles.hpp"
#include "LogFile.hpp"
#include "backend.hpp"
#include "ProgressBar.hpp"
#include "Decom.hpp"

namespace h5 = h5cpp;

/**
 * Returns list of choices for decom. Passed to Qt for user selection.
 *
 * @param directory Directory to look in for files.
 * @param backend Qt backend pointer for setting satellite type.
 * @return Set containing the file options.
 */
std::set<std::string> h5Decode::getFileTypeNames(const std::string& directory, BackEnd* backend)
{
    m_directory = directory;
    std::set<std::string> outfileNames;
    m_files = getFiles::filesInDirectory(m_directory, ".h5");  // Get all matching files
    if (m_files.size() == 0)
    {
        LogFile::logError("No .h5 files found");
        exit(0);
    }

    for (const auto& filename : m_files)
    {
        h5::File h5File(filename, "r");
        m_h5Files.emplace_back(h5File);
        h5::Group All_Data = h5File.root().open_group("All_Data");

        for (size_t group = 0; group < All_Data.size(); ++group)
        {
            outfileNames.emplace(All_Data.get_link_name(group));  // Get internal file name from h5 file
        }
    }

    sortFiles();
    backend->setType(checkType(m_files.front()));
    // This creates a file called datesFile.dat so that matlab can see the dates and SCIDs in the output txt
    std::string input =  m_files.front() + m_files.back();  // We can put this on a single line. It does not matter for Matlab
    std::ofstream datesFile;
    datesFile.open("output/datesFile.dat");  // create a file with the first and last dates for matlab to use in creating a directory structure by SCID and date
    datesFile << input;
    datesFile.close();

    return outfileNames;
}


/**
 * Main h5Decode/Decom function. Read h5 files contents into vectors, which are then passed by queue to a thread resposible for running the Decom class.
 *
 * @param backend Pointer to Qt backend for callbacks and status updates.
 * @param selectedFiles List of user selected files, used for skipping unselected files.
 * @param debug Debug flag.
 * @param entries Database entries to be passed to the Decom class.
 * @param type Satellite type.
 * @param bigEndian Endian format of input data.
 */
void h5Decode::init(BackEnd* backend, const std::vector<std::string>& selectedFiles, const bool& debug, const std::vector<DataTypes::Entry>& entries, const DataTypes::SCType& type, const bool& bigEndian)
{
    Decom decomEngine(debug, entries, type, bigEndian);  // Create Decom object
    std::thread decomThread(&Decom::init, decomEngine, &m_queue, backend);  // Start Decom init function in a another thread
    decomThread.detach();
    ProgressBar pbar(m_files.size(), "Parsing h5");
    uint32_t i = 0;
    for (auto& h5File : m_h5Files)  // For each h5 file
    {
        backend->setCurrentFile(h5File.get_file_name());  // Notify backend of the name of the file we are parsing
        pbar.Progressed(++i, backend);

        h5::Group All_Data = h5File.root().open_group("All_Data");

        for (size_t group = 0; group < All_Data.size(); ++group)
        {
            std::string APgroupString = All_Data.get_link_name(group);
            std::string instrument;
            size_t found;
            if ((found = APgroupString.find("-")) != std::string::npos)  // Get instrument from filename
                instrument = APgroupString.substr(0, found);

            if (std::find(std::begin(selectedFiles), std::end(selectedFiles), APgroupString) != std::end(selectedFiles))  // If this is a selected file
            {
                h5::Group APgroup = (All_Data.open_group(APgroupString));

                std::vector<uint8_t> allData;

                for (size_t AP = 0; AP < APgroup.size(); ++AP)
                {
                    h5::Dataset RawAP = APgroup.open_dataset(APgroup.get_link_name(AP));
                    std::vector<uint8_t> data;
                    h5::read_dataset<uint8_t>(RawAP, data);  // Read h5 data into vector of bytes

                    int32_t apStorageOffset = static_cast<int32_t>(data.at(51)) + (static_cast<int32_t>(data.at(50)) * 256) + (static_cast<int32_t>(data.at(49)) * 65536) + (static_cast<int32_t>(data.at(48)) * 16777216);  // location from RDR Static Header table in CDFCB Vol 2
                    allData.insert(std::end(allData), std::make_move_iterator(std::begin(data) + apStorageOffset), std::make_move_iterator(std::end(data)));  // Move local vector into allData
                }
                m_queue.push(std::make_tuple(allData, instrument));  // Pass the h5 file contents to Decom
            }
        }
        h5File.close();
    }
    backend->setProgress("Waiting for writer threads to finish.");
    m_queue.setInactive();  // Notify Decom that no more h5 files will be pushed onto the queue
}

/**
 * Sorts h5 files based on time.
 */
void h5Decode::sortFiles()
{
    auto sortLambda = [] (const std::string& a, const std::string& b) -> bool
                      {
                          std::vector<std::string> astrings;
                          std::vector<std::string> bstrings;
                          boost::split(astrings, a, boost::is_any_of("_"));
                          boost::split(bstrings, b, boost::is_any_of("_"));
                          std::string aday;
                          std::string bday;
                          std::string amin;
                          std::string bmin;
                          std::string asec;
                          std::string bsec;
                          for (const auto& s : astrings)
                          {
                              if (s.substr(0, 2) == "d2")  // Year of d2xxx
                                  aday = s;
                              if (s.at(0) == 't')
                                  amin = s;
                              if (s.at(0) == 'e')
                                  asec = s;
                          }
                          for (const auto& s : bstrings)
                          {
                              if (s.substr(0, 2) == "d2")  // Year of d2xxx
                                  bday = s;
                              if (s.at(0) == 't')
                                  bmin = s;
                              if (s.at(0) == 'e')
                                  bsec = s;
                          }
                          if (aday != bday)
                              return aday < bday;
                          else
                          {
                              if (amin != bmin)
                                  return amin < bmin;
                              else
                                  return asec < bsec;
                          }
                      };
    std::sort(std::begin(m_files), std::end(m_files), sortLambda);
}

/**
 * Given a h5 filename, extract the mission type (NPP, J01, etc.).
 *
 * @param filename Path of the file to check.
 * @return Enum containg satellite type.
 */
DataTypes::SCType h5Decode::checkType(const std::string& filename)
{
    boost::filesystem::path filepath(filename);  // Convert to boost path so that we can strip the path info
    std::vector<std::string> splitstring;
    boost::split(splitstring, filepath.stem().string(), boost::is_any_of("_"));  // Split string on the underscore
    if (splitstring.at(1) == "npp")
        return DataTypes::SCType::NPP;
    else if (splitstring.at(1) == "j01")
        return DataTypes::SCType::J1;
    else if (splitstring.at(1) == "j02")
        return DataTypes::SCType::J2;
    else if (splitstring.at(1) == "j03")
        return DataTypes::SCType::J3;
    else
        return DataTypes::SCType::J4;
}
