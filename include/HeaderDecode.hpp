#pragma once
#include <tuple>
#include <sstream>
#include "DataTypes.hpp"

namespace HeaderDecode
{
    const struct DataTypes::PrimaryHeader p_defaults = { DataTypes::STANDALONE, 0, 0, 0, 0, 0};
    const struct DataTypes::SecondaryHeader s_defaults = {0, 0, 0, 0};

    std::tuple<DataTypes::PrimaryHeader, DataTypes::SecondaryHeader, bool> decodeHeaders(std::istringstream& buffer, const bool& debug);

    void debugPrinter(const DataTypes::PrimaryHeader& ph);

    DataTypes::PrimaryHeader decodePrimary(std::istringstream& buffer, const bool& debug);

    DataTypes::SecondaryHeader decodeSecondary(std::istringstream& buffer);

    void checkValidHeader(const DataTypes::PrimaryHeader& pheader);
}
