#pragma once

#include <string>
#include <fstream>
#include <set>
#include <vector>
#include <unordered_map>
#include <tuple>
#include "DataTypes.hpp"
#include "ThreadSafeListenerQueue.hpp"

class BackEnd;

class pdsDecode
{
  public:
  pdsDecode() :
    m_directory{}
    {}

    virtual ~pdsDecode() {}

    std::set<std::string> getFileTypeNames(const std::string& directory);
    void init(BackEnd* backend, const std::vector<std::string>& selectedFiles, const bool& debug, const std::vector<DataTypes::Entry>& entries, const DataTypes::SCType& type);
  private:
    std::string m_directory;
    std::unordered_map<std::string, std::ofstream> m_outfiles;
    ThreadSafeListenerQueue<std::tuple<std::vector<uint8_t>, std::string>> m_queue;
    std::vector<std::string> m_files;

    std::ofstream& getStream(const std::string& outfilename);
    std::string getFileName(const std::string& filename);
};
