#pragma once

#include <atomic>
#include <map>
#include <utility>
#include <vector>
#include <cstdint>
#include <string>
#include <optional>
#include <iostream>
#include <memory>
#include <shared_mutex>
#include "include/skip_list.h"

// Define comparison function for keys
struct KeyCompare
{
  bool operator()(const std::pair<std::string, std::string> &lhs, const std::pair<std::string, std::string> &rhs) const
  {
    return lhs.first < rhs.first;
  }
};

class MemTableIterator;

class MemTable
{
private:
  // THIS IS NOT CONCURRENT
  std::shared_ptr<goodliffe::skip_list<std::pair<std::string, std::string>, KeyCompare>> map;
  size_t id;
  std::shared_ptr<std::atomic<size_t>> approximate_size;
  std::shared_mutex mutex;

public:
  MemTable(size_t id);
  std::optional<std::string> Get(const std::string &key);
  void Put(const std::string &key, const std::string &value);
  void Delete(const std::string &key);
  size_t GetId() const;
  size_t ApproximateSize() const;
  MemTableIterator Scan(std::string lower_bound, std::string upper_bound);
};

class MemTableIterator
{
private:
  using SkipListType = goodliffe::skip_list<std::pair<std::string, std::string>, KeyCompare>;
  using SkipListIterator = typename SkipListType::iterator;

  std::shared_ptr<SkipListType> map;
  mutable SkipListIterator it;
  mutable SkipListIterator end;

public:
  MemTableIterator() = default;
  MemTableIterator(std::shared_ptr<SkipListType> map, SkipListIterator it, SkipListIterator end) : map(map), it(it), end(end) {}

  // Get the key of the current element
  std::string Key() const 
  {
    return (*it).first;
  }

  // Get the value of the current element
  std::string Value() const
  {
    return (*it).second;
  }

  // Move to the next element
  void Next()
  {
    ++it;
  }

  // Check if the iterator is still valid
  bool IsValid()
  {
    return it != end;
  }
};
