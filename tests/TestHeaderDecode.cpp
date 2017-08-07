#include "catch/catch.hpp"
#include "HeaderDecode.hpp"

bool operator==(const DataTypes::PrimaryHeader& lhs, const DataTypes::PrimaryHeader& rhs)
{
    if (lhs.sequenceFlag == rhs.sequenceFlag)
        if (lhs.APID == rhs.APID)
            if (lhs.packetSequence == rhs.packetSequence)
                if (lhs.packetLength == rhs.packetLength)
                    if (lhs.secondaryHeader == rhs.secondaryHeader)
                        if (lhs.CCSDS == rhs.CCSDS)
                            return true;
    return false;
}


bool operator==(const DataTypes::SecondaryHeader& lhs, const DataTypes::SecondaryHeader& rhs)
{
    if (lhs.day == rhs.day)
        if (lhs.millis == rhs.millis)
            if (lhs.micros == rhs.micros)
                if (lhs.segments == rhs.segments)
                    return true;
    return false;
}
TEST_CASE("HeaderDecode Checks")
{
    SECTION("Decode Primary")
    {
        std::vector<uint8_t> input = {0x0A, 0x30, 0x76, 0xDC, 0x03, 0xF9};
        std::istringstream input_stream;
        input_stream.rdbuf()->pubsetbuf(reinterpret_cast<char*>(&input[0]), input.size());

        DataTypes::PrimaryHeader ph = HeaderDecode::decodePrimary(input_stream, false, true);
        DataTypes::PrimaryHeader expected = {DataTypes::FIRST, 560, 14044, 1008, 1, 0};
        REQUIRE(ph == expected);
    }
    SECTION("Decode Secondary")
    {
        std::vector<uint8_t> input = {0x54, 0xF7, 0x01, 0xC2, 0x1F, 0x57, 0x01, 0x35, 0x1D, 0x00};

        std::istringstream input_stream;
        input_stream.rdbuf()->pubsetbuf(reinterpret_cast<char*>(&input[0]), input.size());

        DataTypes::SecondaryHeader sh = HeaderDecode::decodeSecondary(input_stream, true);
        DataTypes::SecondaryHeader expected = {21751, 29499223, 309, 29};
        REQUIRE(sh == expected);
    }
}
