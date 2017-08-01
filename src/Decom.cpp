#include <iostream>
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include <string>
#include <memory>
#include <bitset>
#include <sstream>
#include "Decom.h"
#include "ProgressBar.h"
#include "ByteManipulation.h"
#include "HeaderDecode.h"
#include "DataDecode.h"
#include "InstrumentFormat.h"
#include "ReadFile.h"
#include "LogFile.h"
#include "backend.h"

/**
 * Decom engine initializer. Main event loop for calling all Decom helper functions. Stops reading upon reaching end of input file. Writes data once finished reading.
 * Handles instrument formatting after writing (if necessary).
 *
 * @param infile File to read from
 * @return N/A
 */

void Decom::init(ThreadSafeListenerQueue<std::tuple<std::vector<uint8_t>, std::string>>& queue, BackEnd* backend)
{
    ThreadPoolServer pool;  // Create thread pool that we will be passing our packets to

    while (true)
    {
        std::tuple<std::vector<uint8_t>, std::string> queueVal;
        uint32_t retVal = queue.listen(queueVal);
        if (retVal)
        {
            std::istringstream input_stream(std::string(std::begin(std::get<0>(queueVal)), std::end(std::get<0>(queueVal))));
            int64_t fileSize = getFileSize(input_stream);
            while (true)  // Loop until error or we reach end of file
            {
                m_progress = input_stream.tellg();  // Get current progress
                if (input_stream.eof() || m_progress >= fileSize)  // If reached end of file
                    break;
                if (!getHeadersAndEntries(input_stream))  // If invalid header
                    break;
                DataTypes::Packet pack = decodeData(input_stream, std::get<1>(queueVal));

                storeAPID(pack.apid);
                pool.exec(std::make_unique<DataTypes::Packet>(pack), std::get<1>(queueVal));  // Push packet into our writer thread's queue
            }
        }
        else
        {
            pool.join();  // Wait for writer threads to join
            formatInstruments(backend); // Check if we need to format instrument data
            backend->setFinished();
            break;
        }
    }
}

/**
 * Finds database entries that match our current APID.
 *
 * @param APID APID to search for
 * @return N/A
 */
void Decom::getEntries(const uint32_t& APID)
{
    if (std::find(std::begin(m_missingAPIDs), std::end(m_missingAPIDs), APID) != std::end(m_missingAPIDs)){  // If we already determined apid was missing, then skip
        return;
    }

    if (m_mapEntries[APID].empty())  // If we haven't already loaded entries for this apid
    {
        bool foundEntry = false;
        for (const auto& entry : m_entries)  // Loop through all entries
        {
            if (entry.i_APID == APID)
            {
                if (!entry.ignored)
                {
                    m_mapEntries[APID].emplace_back(entry);
                }
                foundEntry = true;
            }
        }
        if (!foundEntry)
        {
            m_missingAPIDs.emplace_back(APID);
            LogFile::logError("Couldn't find matching APID in database: " + std::to_string(APID));
        }
    }
}

/**
 * Handles any special formatting requirements for instrument science data.
 *
 * @return N/A
 */
void Decom::formatInstruments(BackEnd* backend) const
{
    if(m_APIDs.count(528))  // 528 is ATMS science apid
        InstrumentFormat::formatATMS(backend);
}

/**
 * Stores set of all APIDs that we have processed.
 *
 * @param APID APID to store
 * @return N/A
 */
void Decom::storeAPID(const uint32_t& APID)
{
    m_APIDs.insert(APID);
}

/**
 * Finds filesize by seeking to end of file and getting file pointer position.
 *
 * @return fileSize
 */
int64_t Decom::getFileSize(std::istringstream& buffer)
{
    buffer.seekg(0, std::ios::end);  // Seek to end to get filesize
    int64_t fileSize = buffer.tellg();
    buffer.seekg(0, std::ios::beg);
    return fileSize;
}

/**
 * Create DataDecode object and call correct function for decoding data.
 *
 * @return Packet struct containing the data
 */
DataTypes::Packet Decom::decodeData(std::istringstream& buffer, const std::string& instrument)
{
    DataDecode dc{std::get<0>(m_headers), std::get<1>(m_headers), m_mapEntries[std::get<0>(m_headers).APID], m_debug, instrument, m_NPP};  // Create new dataDecode object and pass headers/instrument info

    if (instrument == "OMPS")  // If omps then use special function
    {
        return dc.decodeOMPS(buffer);
    }
    else if (std::get<0>(m_headers).sequenceFlag == DataTypes::FIRST)  // If segmented packet
    {
        return dc.decodeDataSegmented(buffer, false);
    }
    else  // Otherwise standalone packet
    {
        return dc.decodeData(buffer, 0);
    }
}

/**
 * Calls decodeHeaders function and stores result in member variable.
 * Once complete finds matching entries.
 *
 * @return False on invalid header
 */
bool Decom::getHeadersAndEntries(std::istringstream& buffer)
{
    m_headers = HeaderDecode::decodeHeaders(buffer, m_debug);  // Decode headers

    if (!std::get<2>(m_headers))  // If header is invalid
        return false;

    getEntries(std::get<0>(m_headers).APID);  // Get matching database entries from header APID
    return true;
}
