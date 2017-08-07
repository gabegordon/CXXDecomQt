#include "catch/catch.hpp"
#include "DatabaseReader.hpp"

bool operator==(const DataTypes::Entry& lhs, const DataTypes::Entry& rhs)
{
    if (lhs.mnemonic == rhs.mnemonic)
        if (lhs.type == rhs.type)
            if (lhs.i_APID == rhs.i_APID)
                if (lhs.byte == rhs.byte)
                    if (lhs.bitLower == rhs.bitLower)
                        if (lhs.bitUpper == rhs.bitUpper)
                            if (lhs.length == rhs.length)
                                if (lhs.ignored == rhs.ignored)
                                    return true;
    return false;
}
TEST_CASE("DatabaseReader Checks")
{
    SECTION("Database All APIDS")
    {
        std::vector<uint32_t> apids;
        DatabaseReader dr(true, DataTypes::J1, apids);
        std::vector<DataTypes::Entry> results;

        results = dr.getEntries();
        DataTypes::Entry result = results.at(0);
        DataTypes::Entry expected = {"TEST2", DataTypes::UNSIGNED, 0, 6, 0, 2, 3, false};
        REQUIRE(result == expected);
    }
    SECTION("Database APID Filter Included")
    {
        std::vector<uint32_t> apids = { 0 };
        DatabaseReader dr(false, DataTypes::J1, apids);
        std::vector<DataTypes::Entry> results;

        results = dr.getEntries();
        DataTypes::Entry result = results.at(0);
        DataTypes::Entry expected = {"TEST2", DataTypes::UNSIGNED, 0, 6, 0, 2, 3, false};
        REQUIRE(result == expected);
    }
    SECTION("Database APID Filter Ignored")
    {
        std::vector<uint32_t> apids;
        DatabaseReader dr(false, DataTypes::J1, apids);
        std::vector<DataTypes::Entry> results;

        results = dr.getEntries();
        DataTypes::Entry result = results.at(0);
        DataTypes::Entry expected = {"TEST2", DataTypes::UNSIGNED, 0, 6, 0, 2, 3, true};
        REQUIRE(result == expected);

    }
}
