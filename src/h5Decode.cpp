#include <iostream>
#include <vector>
#include <cstdint>
#include <set>
#include "h5Decode.h"
#include "getFiles.h"
#include "hdf_wrapper.h"
#include "LogFile.h"

namespace h5 = h5cpp;

/**
 * Main h5 decode function. Reads all h5's in folder and writes packet files.
 *
 * @return set of files written
 */
std::set<std::string> h5Decode::init()
{
    std::set<std::string> outfileNames;
    auto files = getFiles::h5InFolder(m_directory);
    if(files.size() == 0)
    {
        LogFile::logError("No .h5 files found");
        exit(0);
    }

    // TODO  Sort Files
    for (const auto& filename : files)
    {
        h5::File h5File(filename, "r");
        h5::Group All_Data = h5File.root().open_group("All_Data");

        for(size_t group = 0; group < All_Data.size(); ++group)
        {
            std::string APgroupString = All_Data.get_link_name(group);
            h5::Group APgroup = (All_Data.open_group(APgroupString));

            std::vector<uint8_t> allData;

            for (size_t AP = 0; AP < APgroup.size(); ++AP)
            {
                h5::Dataset RawAP = APgroup.open_dataset(APgroup.get_link_name(AP));
                std::vector<uint8_t> data;
                h5::read_dataset<uint8_t>(RawAP, data);

                int32_t apStorageOffset = static_cast<int32_t>(data.at(51)) + (static_cast<int32_t>(data.at(50)) * 256) + (static_cast<int32_t>(data.at(49)) * 65536) + (static_cast<int32_t>(data.at(48)) * 16777216);
                data.erase(std::begin(data), (std::begin(data) + apStorageOffset));
                allData.insert(std::end(allData), std::begin(data), std::end(data));
            }
            writeFile(APgroupString, allData);
            outfileNames.emplace(APgroupString);
        }
    }
    for(auto& ofile : m_outfiles)
        ofile.second.close();
    return outfileNames;
}

/**
 * Writes a packet file from h5 data.
 *
 * @param child Name of file to write
 * @param data Data to write
 * @return N/A
 */
void h5Decode::writeFile(const std::string& child, const std::vector<uint8_t>& data)
{
    std::ofstream& ofile = getStream(child);
    for (const uint8_t& byte : data)
    {
        ofile << byte;
    }
}

/**
 * Gets correct output file and returns stream.
 *
 * @param child Filename
 * @return ofstream reference
 */
std::ofstream& h5Decode::getStream(const std::string& child)
{
    auto& ofile = m_outfiles[child];
    if(!ofile.is_open())
    {
        ofile.open("output/" + child, std::ios::binary);
    }
    return ofile;
}
