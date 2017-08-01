#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include "DatabaseReader.hpp"
#include "DataTypes.hpp"
#include "CSVRow.hpp"
#include "ReadFile.hpp"
#include "LogFile.hpp"

/**
 * Overide stream operator for reading from our CSVRow class.
 *
 * @param str Stream to read from.
 * @param data Our CSVRow object.
 * @return Stream containing row.
 */
std::istream& operator >> (std::istream& str, CSVRow& data)
{
    data.readNextRow(str);
    return str;
}

/**
 * Helper function to read all necessary databases.
 */
void DatabaseReader::init()
{
    switch (m_type)
    {
    case DataTypes::SCType::NPP:
    {
        readDatabase("databases/nppdatabase.csv");
        break;
    }
    case DataTypes::SCType::J1:
    {
        readDatabase("databases/j1database.csv");
        break;
    }
    case DataTypes::SCType::J2:
    {
        readDatabase("databases/j2database.csv");
        break;
    }
    case DataTypes::SCType::J3:
    {
        readDatabase("databases/j3database.csv");
        break;
    }
    case DataTypes::SCType::J4:
    {
        readDatabase("databases/j4database.csv");
        break;
    }
    }
    readDatabase("databases/atmsdatabase.csv");
    readDatabase("databases/ompsdatabase.csv");
    readDatabase("databases/ceresdatabase.csv");
}

/**
 * Extracts byte and bit integers out of byte:bit string from database column.
 *
 * @param bytebit "byte:bit" string read from database.
 * @param i_byte Unsigned 32-bit integer to be set by function.
 * @param i_bitLower Unsigned 32-bit integer to be set by function.
 * @param i_bitUpper Unsigned 32-bit integer to be set by function.
 * @return None. Parameters serve as the return value.
 */
void DatabaseReader::getByteBit(std::string* bytebit, uint32_t* i_byte, uint32_t* i_bitLower, uint32_t* i_bitUpper) const
{
    bytebit->erase(0, 1);  // Remove leading /
    bytebit->erase(std::remove_if(bytebit->begin(), bytebit->end(), isspace), bytebit->end());  // Remove any whitespace from the string
    std::string s_byte = bytebit->substr(0, 4);
    try
    {
        *i_byte = std::stoi(s_byte);
    }
    catch(...)
    {
        LogFile::logError("stoi failed for i_byte: " + *bytebit);
    }
    if (bytebit->length() > 4)  // If longer than 4, then we have a bit range
    {
        std::string s_bit = bytebit->substr(5);  // Substring to end of string
        if (s_bit.length() > 1)
        {
            size_t found;
            if ((found = s_bit.find("-")) != std::string::npos)  // Split (x-y) range
            {
                try
                {
                    *i_bitLower = std::stoi(s_bit.substr(0, found));
                    *i_bitUpper = std::stoi(s_bit.substr(found + 1));
                }
                catch(...)
                {
                    LogFile::logError("stoi failed for i_bitLower or i_bitUpper: " + *bytebit);
                }
            }
        }
        else  // Otherwise we have just a single bit
        {
            try
            {
                *i_bitLower = std::stoi(s_bit);
            }
            catch (...)
            {
                LogFile::logError("i_bitLower stoi failed with: " + *bytebit);
            }
            i_bitUpper = i_bitLower;
        }
    }
}


/**
 * Read a database file. Generate a DataTypes::Entry struct for each row.
 *
 * @param filename Database file to open.
 */
void DatabaseReader::readDatabase(const std::string& filename)
{
    std::ifstream database(filename);
    ReadFiles::checkFile(database, filename);

    CSVRow dataRow;
    while (database >> dataRow)
    {
        if (m_firstRun)  // Skip header row
        {
            m_firstRun = false;
            continue;
        }

        std::string mnem = dataRow[0];
        std::string s_APID = dataRow[2];
        std::string bytebit = dataRow[3];
        DataTypes::Entry tmp = defaults;
        uint32_t i_APID = 0;

        s_APID.erase(0, 4);  // Remove APID string from ex. (APID0001)

        try
        {
            i_APID = std::stoul(s_APID);
        }
        catch (...)
        {
            LogFile::logError("i_APID stoul failed for: " + s_APID);
        }

        if (!m_allAPIDs)  // If filtering APIDs
        {
            if (std::find(m_APIDs.begin(), m_APIDs.end(), i_APID) == m_APIDs.end())  // If not found in selected vector
            {
                tmp.ignored = true;
            }
        }

        tmp.mnemonic = mnem;
        tmp.type = DataTypes::hashIt(dataRow[1].substr(0, 1));
        tmp.i_APID = i_APID;

        uint32_t i_byte = 0;
        uint32_t i_bitLower = 0;
        uint32_t i_bitUpper = 0;
        getByteBit(&bytebit, &i_byte, &i_bitLower, &i_bitUpper);

        tmp.byte = i_byte;
        tmp.bitLower = i_bitLower;
        tmp.bitUpper = i_bitUpper;
        tmp.length = std::stoi(dataRow[1].substr(1, std::string::npos));

        if (bannedAPID(mnem))  // Skip entries containing header info, as we already decode it.
        {
            tmp.ignored = true;
        }

        m_entries.emplace_back(tmp);
    }
    m_firstRun = true;
}

/**
 * Debug function to print what contents were read from the database.
 */
void DatabaseReader::printDataBase() const
{
    for (const auto& entry : m_entries)
    {
        std::cerr << entry.mnemonic << "," << entry.type << "," << entry.i_APID << "," << entry.byte << "," << entry.bitLower << "," << entry.bitUpper << "\n";
    }
}

/**
 * Getter function for member vector containing all entries.
 * Output is sorted before being returned.
 *
 * @return Sorted Entry vector
 */
std::vector<DataTypes::Entry> DatabaseReader::getEntries()
{
    auto sortLambda = [] (const DataTypes::Entry& a, const DataTypes::Entry& b) -> bool
                      {
                          if (a.i_APID != b.i_APID)
                              return a.i_APID < b.i_APID;
                          else
                          {
                              if (a.byte != b.byte)
                                  return a.byte < b.byte;
                              else
                                  return a.bitLower < b.bitLower;
                          }
                      };
    std::sort(m_entries.begin(), m_entries.end(), sortLambda);
    return m_entries;
}

/**
 * Database contains entries for decoding Primary and Secondary header information. As this information is already hardcoded, these entries can be ignored.
 * This function skips entries containing header information.
 * @param mnem Mnemonic to check
 * @return True if we should skip this mnemonic
 */
bool DatabaseReader::bannedAPID(const std::string& mnem) const
{
    std::string strippedmnem;
    if (mnem.length() > 6)
        strippedmnem = mnem.substr(0, 1) + mnem.substr(5);
    else
        return false;
    if (std::find(m_skip.begin(), m_skip.end(), strippedmnem) != m_skip.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}
