#pragma once

#include <string>
#include <fstream>
#include <set>
#include <vector>
#include <unordered_map>
#include "DataTypes.h"
#include "ThreadSafeListenerQueue.h"

class BackEnd;

class pdsDecode
{
  public:
  pdsDecode() :
    m_directory{}
    {}

    virtual ~pdsDecode() {}

    std::set<std::string> getFileTypeNames(const std::string& directory);
    void init(BackEnd* backend, const std::vector<std::string>& selectedFiles, const bool& debug, const std::vector<DataTypes::Entry>& entries, const bool& NPP);
  private:
    std::string m_directory;
    std::unordered_map<std::string, std::ofstream> m_outfiles;
    ThreadSafeListenerQueue<std::tuple<std::vector<uint8_t>, std::string>> m_queue;

    std::ofstream& getStream(const std::string& outfilename);
    std::string getFileName(const std::string& filename);
};
