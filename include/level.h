#pragma once

#include <vector>
#include <memory>
#include "sstable.h"

class Level
{
public:
  Level(const size_t id);
  ~Level();

  void AddSSTable(SSTable* sstable);
  void RemoveSSTable(std::string path);
  std::vector<SSTable*> GetSSTables();
  size_t NextSSTableID();
  void SetNextSSTableID(size_t id);
  void Clear();
  size_t GetID() { return id; }

private:
  size_t id;
  std::vector<SSTable*> sstables;
  size_t next_sstable_id;
};