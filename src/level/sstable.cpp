#include "include/sstable.h"
#include <filesystem>
#include <fstream>
#include <string>

SSTable::~SSTable()
{
  sparse_index.Clear();
  std::filesystem::remove(path);
}

SSTable::SSTable(const std::string &path, const size_t &id, MemTable &memtable, long size) : path(path), id(id), sparse_index(), bloom_filter(size)
{
  std::ofstream sst;
  sst.open(path, std::ios::out | std::ios::binary);

  size_t sparsity_count = 0;
  size_t position = 0;

  std::vector<std::pair<std::string, std::string>> data = memtable.GetAndClear();
  for (auto &pair : data)
  {
    sst << pair.first << "," << pair.second << std::endl;
    if (sparsity_count++ == SPARSITY_FACTOR)
    {
      sparse_index.Put(pair.first, std::to_string(position));
      sparsity_count = 0;
    }
    position += pair.first.size() + pair.second.size() + 1;
    bloom_filter.set(pair.first);
  }
  sst << std::flush;
}

SSTable::SSTable(const std::string &path, const size_t &id, SSTable *sst_a, SSTable *sst_b, long size) : path(path), id(id), sparse_index(), bloom_filter(size)
{
  std::ofstream merged_sst;
  merged_sst.open(path, std::ios::out | std::ios::binary);

  std::ifstream sst_a_file(sst_a->path);
  std::ifstream sst_b_file(sst_b->path);

  size_t sparsity_count = 0;
  size_t position = 0;

  bool sst_a_empty = false;
  bool sst_b_empty = false;

  std::string line_a, line_b;
  std::pair<std::string, std::string> pair_a, pair_b;

  while (true)
  {
    if (line_a.empty() && !sst_a_empty)
    {
      std::getline(sst_a_file, line_a);
      sst_a_empty = line_a.empty();
      size_t pos = line_a.find(",");
      pair_a.first = line_a.substr(0, pos);
      pair_a.second = line_a.substr(pos + 1, line_a.size());
    }

    if (line_b.empty() && !sst_b_empty)
    {
      std::getline(sst_b_file, line_b);
      sst_b_empty = line_b.empty();
      size_t pos = line_b.find(",");
      pair_b.first = line_b.substr(0, pos);
      pair_b.second = line_b.substr(pos + 1, line_b.size());
    }

    if (pair_a.first.empty() && pair_b.first.empty() && pair_a.second.empty() && pair_b.second.empty())
    {
      break;
    }

    std::pair<std::string, std::string> min_pair;
    if (pair_a.first.empty() && !pair_b.first.empty() && pair_a.second.empty() && !pair_b.second.empty())
    {
      min_pair = pair_b;
      line_b.clear();
    }
    else if (!pair_a.first.empty() && pair_b.first.empty() && !pair_a.second.empty() && pair_b.second.empty())
    {
      min_pair = pair_a;
      line_a.clear();
    }
    else if (pair_a.first <= pair_b.first)
    {
      min_pair = pair_a;
      line_a.clear();
    }
    else
    {
      min_pair = pair_b;
      line_b.clear();
    }

    // std::cout << "Merging: " << min_pair.first << " " << min_pair.second << std::endl;
    merged_sst << min_pair.first << "," << min_pair.second << std::endl;
    if (sparsity_count++ == SPARSITY_FACTOR)
    {
      sparse_index.Put(min_pair.first, std::to_string(position));
      sparsity_count = 0;
    }
    position += min_pair.first.size() + min_pair.second.size() + 1;
    bloom_filter.set(min_pair.first);
  }
  merged_sst << std::flush;
}

SSTable::SSTable(const std::string &path, const size_t &id, long size) : path(path), id(id), sparse_index(), bloom_filter(size)
{
  RepopulateInternals(path);
}

void SSTable::RepopulateInternals(const std::string &path)
{
  std::ifstream sst;
  sst.open(path);
  std::string line;
  size_t sparcity_count = 0;
  size_t position = 0;

  while (std::getline(sst, line))
  {
    size_t pos = line.find(",");
    std::string key = line.substr(0, pos);
    std::string value = line.substr(pos + 1, line.size());
    if (sparcity_count++ == SPARSITY_FACTOR)
    {
      sparse_index.Put(key, std::to_string(position));
      sparcity_count = 0;
    }
    bloom_filter.set(key);
    position += line.size();
  }
}

std::string SSTable::Get(const std::string &key)
{
  if (bloom_filter.is_set(key))
  {
    auto value = sparse_index.FindNearest(key);
    size_t position = 0;
    if (value.has_value() && !value.value().empty())
    {
      position = atoi(value.value().c_str());
    }
    std::ifstream sst;
    sst.open(path);
    sst.seekg(position);
    std::string line;
    while (std::getline(sst, line))
    {
      size_t pos = line.find(",");
      std::string k = line.substr(0, pos);
      if (k == key)
      {
        return line.substr(pos + 1, line.size());
      }
      if (k > key)
      {
        break;
      }
    }
  }
  return "";
}

std::string SSTable::GetPath() const
{
  return path;
}

void SSTable::Clear()
{
  sparse_index.Clear();
  std::filesystem::remove(path);
}