#pragma once 


#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include "mem_table.h"

class WAL
{

public:
  WAL(const std::string &filename);

  ~WAL();

  void Append(const std::string &content);

  void Clear();

  void RepopulateMemTable(MemTable* memtable) const;

private:
  std::string filename;
  std::ofstream wal_file;
};