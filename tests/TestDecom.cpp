#include "catch/catch.hpp"
#include "DatabaseReader.hpp"
#include "DataDecode.hpp"
#include "DataTypes.hpp"

bool operator==(const DataTypes::Packet& lhs, const DataTypes::Packet& rhs)
{
    if (lhs.data == rhs.data)
        if (lhs.day == rhs.day)
            if (lhs.millis == rhs.millis)
                if (lhs.micros == rhs.micros)
                    if (lhs.sequenceCount == rhs.sequenceCount)
                        if (lhs.apid == rhs.apid)
                            if (lhs.ignored == rhs.ignored)
                                return true;
    return false;
}

TEST_CASE("DataDecode Checks")
{
    std::vector<uint32_t> apids;
    DatabaseReader dr(true, DataTypes::J1, apids);
    std::vector<DataTypes::Entry> results;
    results = dr.getEntries();
    results.erase(std::begin(results) + 1);
    DataTypes::PrimaryHeader ph = {DataTypes::STANDALONE, 0, 0, 1, 0, 0};
    DataTypes::SecondaryHeader sh = {0, 0, 0, 0};

    DataDecode dc(ph, sh, results, false, "TEST", DataTypes::J1, true);

    std::vector<uint8_t> input = {0xFF};
    std::istringstream input_stream;
    input_stream.rdbuf()->pubsetbuf(reinterpret_cast<char*>(&input[0]), input.size());

    DataTypes::Packet pack = dc.decodeData(input_stream, 0);

    DataTypes::Numeric tmpnum = {DataTypes::Numeric::U32, "TEST2", {.u32 = 7}};
    std::vector<DataTypes::Numeric> tmpvec = {tmpnum};
    DataTypes::Packet expectedpack = {tmpvec, 0, 0, 0, 0, 0, 0};

    REQUIRE(pack == expectedpack);
}
