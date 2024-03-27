#include "include/lsm_tree.h"
#include "include/files.h"
#include <iostream>

LSMTree::LSMTree() : memtable(), wal(std::make_shared<WAL>(WAL_PATH))
{
  // Create levels
  for (size_t i = 0; i < LEVELS; i++)
  {
    levels.push_back(new Level(i));
  }
};

LSMTree::~LSMTree()
{
  memtable.Clear();
  wal->Clear();
}

void LSMTree::DropDB()
{
  DeleteLevels();
  memtable.Clear();
  wal->Clear();
}

void LSMTree::Put(const std::string &key, const std::string &value)
{
  // size_t size = memtable.ApproximateSize()
  if (memtable.ApproximateSize() > MEMTABLE_SIZE)
  {
    Compact();
    FlushMemTable();
    wal->Clear();
  }
  memtable.Put(key, value);
  wal->Append(key + "," + value);
}

std::string LSMTree::Get(const std::string &key)
{
  auto result = memtable.Get(key);
  if (result.has_value() && !result.value().empty())
  {
    return result.value();
  }
  return SearchInSSTables(key);
}

void LSMTree::Delete(const std::string &key)
{
  memtable.Put(key, "");
}

void LSMTree::DeleteLevels()
{
  // Delete all levels
  for (auto &level : levels)
  {
    level->Clear();
  }
  levels.clear();
  memtable.Clear();
}

void LSMTree::FlushMemTable()
{
  // Create a new SSTable from the memtable
  auto next_sstable_id = levels[0]->NextSSTableID();
  auto sstable = new SSTable(SSTABLES_PATH + create_file_name_based_on_level(next_sstable_id, 0), next_sstable_id, memtable, memtable.Size());
  levels[0]->AddSSTable(sstable);
  // MemTable is cleared above
}

/**
 * Merge multiple segments into one.
 * Start with level i = 0 (smallest SST) and merge 2 SSTs at each level and
 * push the merged one to the next level i + 1.
 */
void LSMTree::Compact()
{
  for (size_t i = 0; i < levels.size() - 1; i++)
  {
    while (levels[i]->GetSSTables().size() > 1)
    {
      // std::cout << "Trying to merge at level " << i << std::endl;
      auto sst_a = levels[i]->GetSSTables().back();
      levels[i]->RemoveSSTable(sst_a->GetPath());

      auto sst_b = levels[i]->GetSSTables().back();
      levels[i]->RemoveSSTable(sst_b->GetPath());

      size_t next_sstable_id = levels[i + 1]->NextSSTableID();
      auto merged_sst = new SSTable(SSTABLES_PATH + create_file_name_based_on_level(next_sstable_id, i + 1), next_sstable_id, sst_a, sst_b, (long)memtable.Size() * (i + 1) * 2);
      delete sst_a;
      delete sst_b;

      if (i + 1 != levels.size())
      {
        levels[i + 1]->AddSSTable(merged_sst);
        // std::cout << "Pushed merged sst to above level" << std::endl;
      }
    }
  }
}

std::string LSMTree::SearchInSSTables(const std::string &key)
{
  for (auto &level : levels)
  {
    for (auto &sstable : level->GetSSTables())
    {
      auto result = sstable->Get(key);
      if (!result.empty())
      {
        return result;
      }
    }
  }
  return "";
}

void LSMTree::Recover()
{
  // Store the maximum SSTable ID for each level
  std::vector<size_t> max_sstable_id(levels.size(), 0);

  for (const auto &sstable_file : std::filesystem::directory_iterator(SSTABLES_PATH))
  {
    std::string sstable_file_path = sstable_file.path().string();
    std::string filename = sstable_file_path.substr(sstable_file_path.find_last_of("/\\") + 1);
    auto id_level_id = extract_ids_level_from_file_name(filename);

    size_t level_order = id_level_id.second;

    // Create a new SSTable instance
    auto *sst = new SSTable(sstable_file_path, id_level_id.first, (long)MEMTABLE_SIZE * (level_order + 1) * 2);
    levels[level_order]->AddSSTable(sst);

    // Update the maximum SSTable ID for the current level
    max_sstable_id[level_order] = std::max(max_sstable_id[level_order], id_level_id.first);
  }
  
  for (size_t i = 0; i < levels.size(); ++i)
  {
    levels[i]->SetNextSSTableID(max_sstable_id[i] + 1);
  }
  wal->RepopulateMemTable(&memtable);
}