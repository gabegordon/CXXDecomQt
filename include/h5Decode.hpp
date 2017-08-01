#pragma once

#include <string>
#include <unordered_map>
#include <fstream>
#include <tuple>
#include <set>
#include <vector>
#include "DataTypes.hpp"
#include "ThreadSafeListenerQueue.hpp"
#include "hdf_wrapper.hpp"

class BackEnd;

class h5Decode
{
  public:
  h5Decode() :
    m_directory{},
    m_outfiles{}
    {}

    virtual ~h5Decode() {}

    std::set<std::string> getFileTypeNames(const std::string& directory, DataTypes::SCType& type);
    void init(BackEnd* backend, const std::vector<std::string>& selectedFiles, const bool& debug, const std::vector<DataTypes::Entry>& entries, const DataTypes::SCType& type);
  private:
    std::string m_directory;
    std::unordered_map<std::string, std::ofstream> m_outfiles;
    ThreadSafeListenerQueue<std::tuple<std::vector<uint8_t>, std::string>> m_queue;
    std::vector<h5cpp::File> m_h5Files;
    std::vector<std::string> m_files;

    void sortFiles(std::vector<std::string>& files);
    DataTypes::SCType checkType(const std::string& filename);
};
