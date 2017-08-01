#pragma once
#include <vector>
#include <set>
#include <unordered_map>
#include <fstream>
#include <tuple>
#include <string>
#include <sstream>
#include "DataTypes.h"
#include "ThreadPoolServer.h"

class BackEnd;

typedef uint8_t BYTE;

class Decom
{
  public:
  Decom(const bool& debug, const std::vector<DataTypes::Entry>& entries, const DataTypes::SCType& type) :
    m_mapEntries{},
    m_entries{entries},
    m_progress{0},
    m_debug{debug},
    m_headers{},
    m_type{type}
    {};
    virtual ~Decom() {}

    void init(ThreadSafeListenerQueue<std::tuple<std::vector<uint8_t>, std::string>>& queue, BackEnd* backend);

  private:
    void getEntries(const uint32_t& APID);
    void formatInstruments(BackEnd* backend) const;
    void storeAPID(const uint32_t& APID);
    int64_t getFileSize(std::istringstream& buffer);
    DataTypes::Packet decodeData(std::istringstream& buffer, const std::string& instrument);
    bool getHeadersAndEntries(std::istringstream& buffer);

    std::unordered_map<uint32_t, std::vector<DataTypes::Entry>> m_mapEntries;
    std::vector<DataTypes::Entry> m_entries;

    int64_t m_progress;
    bool m_debug;
    std::vector<uint32_t> m_missingAPIDs;
    std::set<uint32_t> m_APIDs;
    std::tuple<DataTypes::PrimaryHeader, DataTypes::SecondaryHeader, bool> m_headers;
    DataTypes::SCType m_type;
};
