#include "include/wal.h"

WAL::WAL(const std::string &filename) : filename(filename)
{
  wal_file.open(filename, std::ios::app);
}

WAL::~WAL()
{
  wal_file.close();
}

void WAL::Append(const std::string &content)
{
  if (wal_file.is_open()) {
    wal_file << content << std::endl;
  }
}

void WAL::Clear()
{
  std::filesystem::resize_file(filename, 0);
}

void WAL::RepopulateMemTable(MemTable* memtable) const
{
  // Read the WAL file and repopulate the memtable
  std::ifstream wal_file(filename);
  std::string line;
  while (std::getline(wal_file, line))
  {
    size_t pos = line.find(",");
    std::string key = line.substr(0, pos);
    std::string value = line.substr(pos + 1, line.size());
    memtable->Put(key, value);
  }
  wal_file.close();
}