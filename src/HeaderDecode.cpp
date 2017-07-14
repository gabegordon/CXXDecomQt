#include <iostream>
#include <bitset>
#include <tuple>
#include "HeaderDecode.h"
#include "ReadFile.h"
#include "ByteManipulation.h"
#include "LogFile.h"

namespace HeaderDecode
{
uint32_t sh_flag;
DataTypes::SequenceFlag seq_flag;
bool isValid = false;

/**
 * Wrapper function to call primary and secondary decoder functions. Also checks if header is valid.
 *
 * @param infile File to read from
 * @param debug Print debug info flag
 * @return Tuple containing headers and valid flag
 */
std::tuple<DataTypes::PrimaryHeader, DataTypes::SecondaryHeader, bool> decodeHeaders(std::ifstream& infile, const bool& debug)
{
    auto ph = decodePrimary(infile, debug);
    auto sh = decodeSecondary(infile);

    if (!isValid)
    {
        LogFile::logError("Invalid header: ");
        debugPrinter(ph);
    }

    return std::make_tuple(ph, sh, isValid);
}

/**
 * Debug function that prints header contents.
 *
 * @param ph Header to print
 * @return N/A
 */
void debugPrinter(const DataTypes::PrimaryHeader& ph)
{
    LogFile::logError(std::to_string(ph.secondaryHeader) + "," + std::to_string(ph.APID) + "," + std::bitset<2>(ph.sequenceFlag).to_string() + "," + std::to_string(ph.packetSequence) + "," + std::to_string(ph.packetLength));
}

/**
 * Function to handle primary header decoding.
 *
 * @param infile File to read from
 * @param debug Debug flag
 * @return PrimaryHeader struct
 */
DataTypes::PrimaryHeader decodePrimary(std::ifstream& infile, const bool& debug)
{
    DataTypes::PrimaryHeader ph = p_defaults;
    uint32_t firstFourBytes;
    uint16_t fifthSixByte;
    ReadFiles::read(firstFourBytes, infile);
    ReadFiles::read(fifthSixByte, infile);
    firstFourBytes = ByteManipulation::swapEndian32(firstFourBytes);
    fifthSixByte = ByteManipulation::swapEndian16(fifthSixByte);
    // Set CCSDS from bits 0-3
    ph.CCSDS = ByteManipulation::extract32(firstFourBytes, 0, 3);

    // Set secondaryHeader from bit 4
    ph.secondaryHeader = ByteManipulation::extract32(firstFourBytes, 4, 1);
    sh_flag = ph.secondaryHeader;

    // Set APID from bits 4-15
    ph.APID = ByteManipulation::extract32(firstFourBytes, 5, 11);

    // Set sequenceFlag from bits 16-17
    ph.sequenceFlag = static_cast<DataTypes::SequenceFlag>(ByteManipulation::extract32(firstFourBytes, 16, 2));
    seq_flag = ph.sequenceFlag;

    // Set packetSequence from bits 18-31
    ph.packetSequence = ByteManipulation::extract32(firstFourBytes, 18, 14);

    // Set packetLength from entire byte
    ph.packetLength = fifthSixByte + 1;

    if (debug)
        debugPrinter(ph);

    // Account for secondary header length
    if (sh_flag)
    {
        if (seq_flag == DataTypes::FIRST)  // Segmented packet has 2 byte number of grouped packets
            ph.packetLength -= 10;
        else
            ph.packetLength -= 8;  // Otherwise just 8 byte CCSDS time code
    }

    checkValidHeader(ph);
    return ph;
}

/**
 * Function to handle secondary header decoding.
 *
 * @param infile File to read from
 * @return SecondaryHeader struct
 */
DataTypes::SecondaryHeader decodeSecondary(std::ifstream& infile)
{
    DataTypes::SecondaryHeader sh = s_defaults;
    if (sh_flag)  // If secondary header flag is set
    {
        uint16_t day;
        uint32_t millis;
        uint16_t micros;

        ReadFiles::read(day, infile);
        ReadFiles::read(millis, infile);
        ReadFiles::read(micros, infile);

        sh.day = ByteManipulation::swapEndian16(day);
        sh.millis = ByteManipulation::swapEndian32(millis);
        sh.micros = ByteManipulation::swapEndian16(micros);
        if (seq_flag == DataTypes::FIRST)
        {
            // If first segmented packet, then bits 0-7 are segment count
            uint16_t packetSegments;
            ReadFiles::read(packetSegments, infile);
            packetSegments = ByteManipulation::swapEndian16(packetSegments);
            sh.segments = ByteManipulation::extract16(packetSegments, 0, 8);
        }
    }
    return sh;
}

/**
 * Checks if primary header contains valid information.
 *
 * @param pheader Header to check
 * @return N/A
 */
void checkValidHeader(const DataTypes::PrimaryHeader& pheader)
{
    if (pheader.CCSDS != 0)
        isValid = false;
    else if (pheader.APID > 1449)
        isValid = false;
    else if (pheader.packetSequence > 16383)
        isValid = false;
    else if (pheader.packetLength > 65535)
        isValid = false;
    else
        isValid = true;
}
}
