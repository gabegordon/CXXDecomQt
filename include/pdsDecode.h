#pragma once

#include<string>
#include<fstream>
#include<set>
#include<unordered_map>

class BackEnd;

class pdsDecode
{
  public:
  pdsDecode(const std::string& directory) :
    m_directory{directory}
    {}

    virtual ~pdsDecode() {}

    std::set<std::string> init(BackEnd* backend);

  private:
    std::string m_directory;
    std::unordered_map<std::string, std::ofstream> m_outfiles;

    std::ofstream& getStream(const std::string& outfilename);
    std::string getFileName(const std::string& filename);
};
