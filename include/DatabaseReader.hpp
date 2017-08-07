#pragma once
#include <string>
#include <vector>
#include "DataTypes.hpp"

class DatabaseReader
{
  public:
  DatabaseReader(const bool& allAPIDs, const DataTypes::SCType& type, const std::vector<uint32_t>& selectedAPIDs) :
      m_APIDs(selectedAPIDs),
      m_entries(),
      m_firstRun(true),
      m_allAPIDs(allAPIDs),
      m_type(type)
    {
        init();
    }

    virtual ~DatabaseReader() {}

    std::vector<DataTypes::Entry> getEntries();
  private:
    std::vector<uint32_t> m_APIDs;
    std::vector<DataTypes::Entry> m_entries;
    std::vector<std::string> m_skip = {"PVNO", "PPTYPE", "PSHDF", "PID", "PSEGF", "PSCNT", "PLEN", "PTDAY", "PTMS", "PTUS", "ORCV", "OVER", "OTCNT", "OTFLG", "C_RDR_VER", "C_CONT_COUNT", "C_CONT_FLAG", "C_VERSION", "C_TYPE", "C_APID", "C_GRP_FLAGS", "C_2HDR_FLAG", "C_SEQ_COUNT", "C_PKT_LEN", "C_DOY", "C_MSEC", "C_USEC", "CPID", "CKTTIMEDY", "CKTTIMEML", "CKTTIMEMC"};
    bool m_firstRun;
    bool m_allAPIDs;
    DataTypes::SCType m_type;

    void init();
    void readDatabase(const std::string& filename);
    void getByteBit(std::string* bytebit, uint32_t* i_byte, uint32_t* i_bitLower, uint32_t* i_bitUpper) const;
    void printDataBase() const;
    bool bannedAPID(const std::string& mnem) const;

    const struct DataTypes::Entry defaults = {
        "", DataTypes::NILL, 0, 0, 0, 0, 0, false
    };
};
