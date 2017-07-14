#pragma once

#include <string>
#include <unordered_map>
#include <fstream>
#include <set>

class h5Decode
{
  public:
  h5Decode(const std::string& directory) :
    m_directory{directory},
    m_outfiles{}
    {}

    virtual ~h5Decode() {}

    std::set<std::string> init();
  private:
    std::string m_directory;
    std::unordered_map<std::string, std::ofstream> m_outfiles;
    void writeFile(const std::string& child, const std::vector<uint8_t>& data);
    std::ofstream& getStream(const std::string& child);
};
