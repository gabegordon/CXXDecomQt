#pragma once

#include <string>
#include <unordered_map>
#include <fstream>
#include <tuple>
#include <set>
#include "DataTypes.h"
#include "ThreadSafeListenerQueue.h"

class BackEnd;

class h5Decode
{
  public:
  h5Decode() :
    m_directory{},
    m_outfiles{}
    {}

    virtual ~h5Decode() {}

    std::set<std::string> getFileTypeNames(const std::string& directory, bool& NPP);
    void init(BackEnd* backend, const std::vector<std::string>& selectedFiles, const bool& debug, const std::vector<DataTypes::Entry>& entries, const bool& NPP);
  private:
    std::string m_directory;
    std::unordered_map<std::string, std::ofstream> m_outfiles;
    ThreadSafeListenerQueue<std::tuple<std::vector<uint8_t>, std::string>> m_queue;

    void sortFiles(std::vector<std::string>& files);
    bool checkNPP(const std::string& filename);
};
