#include <iostream>
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include <string>
#include <memory>
#include <sstream>
#include "Decom.hpp"
#include "HeaderDecode.hpp"
#include "DataDecode.hpp"
#include "InstrumentFormat.hpp"
#include "LogFile.hpp"
#include "backend.hpp"

/**
 * Decom engine initializer. Main event loop for calling all Decom helper functions. Stops reading upon reaching end of input stream. Passes data read in to a writer thread via queue.
 * Handles instrument formatting after writing (if necessary).
 *
 * @param queue Thread-safe queue containg byte vectors the h5reader pushes into the queue.
 * @param backend Pointer to Qt backend class for writing status messages.
 */

void Decom::init(ThreadSafeListenerQueue<std::tuple<std::vector<uint8_t>, std::string>>* queue, BackEnd* backend)
{
    ThreadPoolServer pool(m_bigEndian);  // Create thread pool that we will be passing our packets to

    while (true)
    {
        std::tuple<std::vector<uint8_t>, std::string> queueVal;
        uint32_t retVal = queue->listen(queueVal);  // Listen on queue until we receieve data from h5Decode
        backend->setProgress("Items left in Queue: " + std::to_string(queue->size()));
        if (retVal)
        {
            std::istringstream input_stream;
            input_stream.rdbuf()->pubsetbuf(reinterpret_cast<char*>(&std::get<0>(queueVal)[0]), std::get<0>(queueVal).size());  // Transform vector into input string stream.

            int64_t fileSize = getFileSize(input_stream);
            while (true)  // Loop until error or we reach end of file
            {
                if (input_stream.eof() || input_stream.tellg() >= fileSize)  // If reached end of file
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
            formatInstruments(backend);  // Check if we need to format instrument data
            backend->setFinished();
            break;
        }
    }
}

/**
 * Finds database entries that match our current APID.
 *
 * @param APID APID to search for.
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
 * @param backend Qt backend pointer allowing instrumentformat functions to print progress messages.
 */
void Decom::formatInstruments(BackEnd* backend) const
{
    if (m_APIDs.count(528))  // 528 is ATMS science apid
        InstrumentFormat::formatATMS(backend);
}

/**
 * Stores set of all APIDs that we have processed.
 *
 * @param APID APID to store.
 */
void Decom::storeAPID(const uint32_t& APID)
{
    m_APIDs.insert(APID);
}

/**
 * Finds filesize by seeking to end of file and getting file pointer position.
 *
 * @param buffer Stream to get size of.
 * @return fileSize.
 */
int64_t Decom::getFileSize(std::istringstream& buffer) const
{
    buffer.seekg(0, std::ios::end);  // Seek to end to get filesize
    int64_t fileSize = buffer.tellg();
    buffer.seekg(0, std::ios::beg);
    return fileSize;
}

/**
 * Create DataDecode object and call correct function for decoding data.
 *
 * @return Packet struct containing the data.
 */
DataTypes::Packet Decom::decodeData(std::istringstream& buffer, const std::string& instrument)
{
    DataDecode dc{std::get<0>(m_headers), std::get<1>(m_headers), m_mapEntries[std::get<0>(m_headers).APID], m_debug, instrument, m_type, m_bigEndian};  // Create new dataDecode object and pass headers/instrument info

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
 * @param buffer Stream containing the binary data.
 * @return False on invalid header.
 */
bool Decom::getHeadersAndEntries(std::istringstream& buffer)
{
    m_headers = HeaderDecode::decodeHeaders(buffer, m_debug, m_bigEndian);  // Decode headers

    if (!std::get<2>(m_headers))  // If header is invalid
        return false;

    getEntries(std::get<0>(m_headers).APID);  // Get matching database entries from header APID
    return true;
}
