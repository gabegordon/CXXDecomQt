#include "catch/catch.hpp"
#include "DatabaseReader.hpp"

TEST_CASE("DatabaseReader Checks")
{
    SECTION("Database All APIDS")
    {
        std::vector<uint32_t> apids;
        DatabaseReader dr(true, DataTypes::J1, apids);
    }
}
