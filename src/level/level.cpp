#include "include/level.h"

Level::Level(const size_t id) : next_sstable_id(0), id(id) {}

Level::~Level()
{
  sstables.clear();
}

void Level::AddSSTable(SSTable *sstable)
{
  sstables.push_back(sstable);
}

void Level::RemoveSSTable(std::string path)
{
  auto it = std::find_if(sstables.begin(), sstables.end(), [path](SSTable *sstable)
                         { return sstable->GetPath() == path; });

  if (it != sstables.end())
  {
    sstables.erase(it);
  }
}

std::vector<SSTable *> Level::GetSSTables()
{
  return sstables;
}

size_t Level::NextSSTableID()
{
  return next_sstable_id++;
}

void Level::SetNextSSTableID(size_t id)
{
  next_sstable_id = id;
}

void Level::Clear()
{
  for (auto &sstable : sstables)
  {
    sstable->Clear();
  }
}