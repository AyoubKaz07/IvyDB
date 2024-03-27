#pragma once
#include <string>
#include <map>
#include <memory>
#include <vector>
#include "mem_table.h"
#include "level.h"
#include "wal.h"

#define MEMTABLE_SIZE 64
#define LEVELS 4
#define WAL_PATH "../storage/wal.log"
#define SSTABLES_PATH "../storage/sstables/"


class LSMTree {
  public:
    LSMTree();
    ~LSMTree();
    void Put(const std::string &key, const std::string &value);
    std::string Get(const std::string &key);
    void Delete(const std::string &key);
    void FlushMemTable();
    void Compact();
    void Recover();
    void DeleteLevels();
    std::string SearchInSSTables(const std::string &key);
    void DropDB();

  private:
    MemTable memtable;
    std::shared_ptr<WAL> wal;
    std::vector<Level*> levels;
};