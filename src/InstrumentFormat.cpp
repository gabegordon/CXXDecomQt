#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include "ProgressBar.hpp"
#include "InstrumentFormat.hpp"
#include "CSVRow.hpp"
#include "ReadFile.hpp"

struct atms_pack
{
    std::string day;
    std::string millis;
    std::string micros;
    float scanangle;
    uint32_t sos_sync;
    std::vector<uint32_t> chans;
};

struct out_pack
{
    std::string day;
    std::string millis;
    std::string micros;
    std::vector<uint32_t> chans = std::vector<uint32_t>(104);
};

namespace InstrumentFormat
{

/**
 * Handles accumulating data from between scans and writing it to file.
 *
 * @param buf Buffer of atms_pack read in from output file.
 */
void writeChans(const std::vector<atms_pack>& buf, BackEnd* backend)
{
    uint64_t i = 0;
    uint64_t bufSize = buf.size();
    ProgressBar writeProgress(bufSize, "Write ATMS");

    std::vector<out_pack> outpacks(22);  // Create vector of outpackets (1 for each ATMS channel)
    std::vector<float> scans(104);   // Create vector for scan angles (105 per complete scan)
    std::vector<std::ofstream> outfiles(22);  // Create of streams (1 for each ATMS channnel)
    uint16_t scanCounter = 1;
    while (i < bufSize)  // Loop until we reach end of buffer
    {
        writeProgress.Progressed(i, backend);
        uint8_t packCounter = 0;

        for (auto& pack : outpacks)  // Add time info to outpacks
        {
            pack.day = buf.at(i).day;
            pack.millis = buf.at(i).millis;
            pack.micros = buf.at(i).micros;
            pack.chans.at(0) = (buf.at(i).chans.at(packCounter++));
        }
        scans.at(0) = buf.at(i).scanangle;
        i++;  // Get scan angle and chans of new scan flag location, and then increment i so that next loop does not stop immediately

        scanCounter = 1;
        for (uint64_t k = i; k < bufSize; k++)
        {
            if (buf.at(k).sos_sync == 0)
            {
                for (uint16_t l = 0; l < 22; l++)  // For each channel get angle and counts
                {
                    try
                    {
                        scans.at(scanCounter) = (buf.at(k).scanangle);
                        outpacks.at(l).chans.at(scanCounter) = (buf.at(k).chans.at(l));
                    }
                    catch(...)
                    {
                        LogFile::logError("Possible gap in data at row " + std::to_string(k));
                        break;
                    }
                }
                scanCounter++;
            }
            else  // If we get new scan flag then stop
            {
                i = k;
                break;
            }
        }

        for (uint16_t channelNumber = 1; channelNumber < 23; channelNumber++)  // Write to files
        {
            std::ofstream& outfile = outfiles.at(channelNumber - 1);  // Get stream from vector

            if (!outfile.is_open())
            {
                std::string filename = "output/ATMS_CHAN" + std::to_string(channelNumber) + ".txt";
                outfile.open(filename);
            }

            auto out = outpacks.at(channelNumber - 1);
            outfile << out.day << "," << out.millis << "," << out.micros << ",";
            for (const float& scan : scans)
                outfile << scan << ",";
            for (const uint32_t& chan : out.chans)
                outfile << chan << ",";
            outfile << "\n";
        }
    }
}

/**
 * Read ATMS science data in so that it can be properly formatted.
 */
void formatATMS(BackEnd* backend)  // TODO Buffered R/W
{
    CSVRow atms_row;
    std::ifstream m_infile;
    m_infile.open("output/ATMS_528.txt", std::ios::ate);
    ReadFiles::checkFile(m_infile, "output/ATMS_528.txt");

    uint64_t fileSize = m_infile.tellg();
    m_infile.seekg(0, std::ios::beg);  // Seek to beginning because we opened at end
    ProgressBar readProgress(fileSize, "Read ATMS");

    bool firstRow = true;
    std::vector<atms_pack> buf;

    while (m_infile >> atms_row)  // Read rows into atms_pack structs
    {
        readProgress.Progressed(m_infile.tellg(), backend);
        if (firstRow)
        {
            firstRow = false;
            continue;
        }
        atms_pack pack;

        pack.day = atms_row[0];
        pack.millis = atms_row[1];
        pack.micros = atms_row[2];
        pack.scanangle = static_cast<float>(0.005493) * static_cast<float>(std::stoul(atms_row[4]));  // Scan angle conversion constant
        pack.sos_sync = std::stoul(atms_row[6]);
        for (uint8_t i = 14; i < 36; ++i)  // Channel counts are in columns 14 to 36
        {
            pack.chans.emplace_back(std::stoul(atms_row[i]));
        }
        buf.emplace_back(pack);
    }
    writeChans(buf, backend);
}

}
