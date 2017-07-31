#include <iostream>
#include <vector>
#include <cstdint>
#include <set>
#include <algorithm>
#include <iterator>
#include <thread>
#include <boost/algorithm/string.hpp>
#include "h5Decode.h"
#include "getFiles.h"
#include "hdf_wrapper.h"
#include "LogFile.h"
#include "backend.h"
#include "ProgressBar.h"
#include "Decom.h"

namespace h5 = h5cpp;

std::set<std::string> h5Decode::getFileTypeNames(const std::string& directory, bool& NPP)
{
    m_directory = directory;
    std::set<std::string> outfileNames;
    auto files = getFiles::filesInDirectory(m_directory, ".h5");
    if(files.size() == 0)
    {
        LogFile::logError("No .h5 files found");
        exit(0);
    }

    for (const auto& filename : files)
    {
        h5::File h5File(filename, "r");
        h5::Group All_Data = h5File.root().open_group("All_Data");

        for(size_t group = 0; group < All_Data.size(); ++group)
        {
            outfileNames.emplace(All_Data.get_link_name(group));
        }
    }

    NPP = checkNPP(files.front());
    // This creates a file called datesFile.dat so that matlab can see the dates and SCIDs in the output txt
    std::string input =  files.front() + files.back();  // We can put this on a single line. It does not matter for Matlab
    std::ofstream datesFile;
    datesFile.open("output/datesFile.dat");  // create a file with the first and last dates for matlab to use in creating a directory structure by SCID and date
    datesFile << input;
    datesFile.close();

    return outfileNames;
}

/**
 * Main h5 decode function. Reads all h5's in folder and writes packet files.
 *
 * @return set of files written
 */
void h5Decode::init(BackEnd* backend, const std::vector<std::string>& selectedFiles, const bool& debug, const std::vector<DataTypes::Entry>& entries, const bool& NPP)
{
    auto files = getFiles::filesInDirectory(m_directory, ".h5");
    if(files.size() == 0)
    {
        LogFile::logError("No .h5 files found");
        exit(0);
    }
    sortFiles(files);

    Decom decomEngine(debug, entries, NPP);
    std::thread decomThread(&Decom::init, decomEngine, std::ref(m_queue));
    ProgressBar pbar(files.size(), "Parsing h5");
    uint32_t i = 0;
    for (const auto& filename : files)
    {
        backend->setCurrentFile(filename);
        pbar.Progressed(++i, backend);
        h5::File h5File(filename, "r");
        h5::Group All_Data = h5File.root().open_group("All_Data");

        for(size_t group = 0; group < All_Data.size(); ++group)
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
                    h5::read_dataset<uint8_t>(RawAP, data);

                    int32_t apStorageOffset = static_cast<int32_t>(data.at(51)) + (static_cast<int32_t>(data.at(50)) * 256) + (static_cast<int32_t>(data.at(49)) * 65536) + (static_cast<int32_t>(data.at(48)) * 16777216);  // location from RDR Static Header table in CDFCB Vol 2
                    allData.insert(std::end(allData), std::make_move_iterator(std::begin(data) + apStorageOffset), std::make_move_iterator(std::end(data)));
                }
                m_queue.push(std::make_tuple(allData, instrument));
            }
        }
        h5File.close();
    }
    m_queue.setInactive();
    decomThread.join();
}

/**
 * @brief Sorts h5 files
 *
 * @param files Vector of files to sort
 */
void h5Decode::sortFiles(std::vector<std::string>& files)
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
                          for (const auto& s: astrings)
                          {
                              if (s.substr(0,2) == "d2")  // Year of d2xxx
                                  aday = s;
                              if (s.at(0) == 't')
                                  amin = s;
                              if (s.at(0) == 'e')
                                  asec = s;
                          }
                          for (const auto& s: bstrings)
                          {
                              if (s.substr(0,2) == "d2")  // Year of d2xxx
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
    std::sort(std::begin(files), std::end(files), sortLambda);
}

bool h5Decode::checkNPP(const std::string& filename)
{
    std::vector<std::string> splitstring;
    boost::split(splitstring, filename, boost::is_any_of("_"));
    if(splitstring.at(1) == "npp")
        return true;
    else
        return false;
}
