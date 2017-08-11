#include <iostream>
#include <bitset>
#include <tuple>
#include "HeaderDecode.hpp"
#include "ReadFile.hpp"
#include "ByteManipulation.hpp"
#include "LogFile.hpp"

namespace HeaderDecode
{
uint32_t sh_flag;
DataTypes::SequenceFlag seq_flag;
bool isValid = false;

/**
 * Wrapper function to call primary and secondary decoder functions. Also checks if header is valid.
 *
 * @param infile File to read from.
 * @param debug Print debug info flag.
 * @param bigEndian Endian format of input data.
 * @return Tuple containing headers and valid flag.
 */
std::tuple<DataTypes::PrimaryHeader, DataTypes::SecondaryHeader, bool> decodeHeaders(std::istringstream& buffer, const bool& debug, const bool& bigEndian)
{
    auto ph = decodePrimary(buffer, debug, bigEndian);
    auto sh = decodeSecondary(buffer, bigEndian);

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
 * @param ph Header to print.
 */
void debugPrinter(const DataTypes::PrimaryHeader& ph)
{
    LogFile::logError(std::to_string(ph.secondaryHeader) + "," + std::to_string(ph.APID) + "," + std::bitset<2>(ph.sequenceFlag).to_string() + "," + std::to_string(ph.packetSequence) + "," + std::to_string(ph.packetLength));
}

/**
 * Function to handle primary header decoding.
 *
 * @param infile File to read from.
 * @param debug Debug flag.
 * @param bigEndian Endian format of input data.
 * @return PrimaryHeader struct.
 */
DataTypes::PrimaryHeader decodePrimary(std::istringstream& buffer, const bool& debug, const bool& bigEndian)
{
    DataTypes::PrimaryHeader ph = p_defaults;
    if(bigEndian)
    {
        uint32_t firstFourBytes;
        uint16_t fifthSixByte;
        ReadFiles::read(firstFourBytes, buffer);
        ReadFiles::read(fifthSixByte, buffer);

        firstFourBytes = ByteManipulation::swapEndian(firstFourBytes);
        fifthSixByte = ByteManipulation::swapEndian(fifthSixByte);
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
    }
    else
    {
        uint16_t b0b1;
        uint16_t b2b3;
        uint16_t fifthSixByte;
        ReadFiles::read(b0b1, buffer);
        ReadFiles::read(b2b3, buffer);
        ReadFiles::read(fifthSixByte, buffer);
        ph.CCSDS = ByteManipulation::extract16(b0b1, 0, 3);
        ph.secondaryHeader = ByteManipulation::extract16(b0b1, 4, 1);
        sh_flag = ph.secondaryHeader;
        ph.APID = ByteManipulation::extract16(b0b1, 5, 11);
        ph.sequenceFlag = static_cast<DataTypes::SequenceFlag>(ByteManipulation::extract16(b2b3, 0, 2));
        seq_flag = ph.sequenceFlag;
        ph.packetSequence = ByteManipulation::extract16(b2b3, 2, 14);
        ph.packetLength = fifthSixByte + 1;
    }
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
 * @param infile File to read from.
 * @param bigEndian Endian format of input data.
 * @retun SecondaryHeader struct.
 */
DataTypes::SecondaryHeader decodeSecondary(std::istringstream& buffer, const bool& bigEndian)
{
    DataTypes::SecondaryHeader sh = s_defaults;
    if (sh_flag)  // If secondary header flag is set
    {
        uint16_t day;
        uint32_t millis;
        uint16_t micros;

        ReadFiles::read(day, buffer);
        ReadFiles::read(millis, buffer);
        ReadFiles::read(micros, buffer);

        if(bigEndian)
        {
            sh.day = ByteManipulation::swapEndian(day);
            sh.millis = ByteManipulation::swapEndian(millis);
            sh.micros = ByteManipulation::swapEndian(micros);
        }
        else
        {
            sh.day = day;
            sh.millis = millis;
            sh.micros = micros;
        }
        if (seq_flag == DataTypes::FIRST)
        {
            // If first segmented packet, then bits 0-7 are segment count
            uint16_t packetSegments;
            ReadFiles::read(packetSegments, buffer);
            if(bigEndian)
                packetSegments = ByteManipulation::swapEndian(packetSegments);
            sh.segments = ByteManipulation::extract16(packetSegments, 0, 8);
        }
    }
    return sh;
}

/**
 * Checks if primary header contains valid information. (CCSDS Packet bounds)
 *
 * @param pheader Header to check.
 */
void checkValidHeader(const DataTypes::PrimaryHeader& pheader)
{
    if (pheader.APID > 1449)
        isValid = false;
    else if (pheader.packetSequence > 16383)
        isValid = false;
    else if (pheader.packetLength > 65535)
        isValid = false;
    else
        isValid = true;
}
}
