#include "catch/catch.hpp"
#include "CSVRow.hpp"

TEST_CASE("CSVRow Checks")
{
        std::ifstream data("../../tests/testdata/testCSV.csv");
        REQUIRE(data.is_open());
        CSVRow row;
        std::vector<std::string> contents;
        std::vector<std::string> expected = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
        while (data >> row)
        {
            for (size_t i = 0; i < row.size(); ++i)
                contents.emplace_back(row[i]);
        }

        REQUIRE(contents.size() == 10);
        REQUIRE(contents[0] == "0");
        REQUIRE(contents == expected);
}
