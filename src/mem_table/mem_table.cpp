#include "include/mem_table.h"
#include <iostream>

MemTable::MemTable(size_t id) : id(id) {
  map = std::make_shared<goodliffe::skip_list<std::pair<std::string, std::string>, KeyCompare>>();
  approximate_size = std::make_shared<std::atomic<size_t>>(0);
}

std::optional<std::string> MemTable::Get(const std::string& key) {
  auto result = map->find(std::make_pair(key, std::string()));
  if (result != map->end()) {
    return (*result).second;
  }
  return std::nullopt;
}


void MemTable::Put(const std::string& key, const std::string& value) {
  size_t estimated_size = key.size() + value.size();
  map->insert(std::make_pair(key, value));
  approximate_size->fetch_add(estimated_size, std::memory_order_relaxed);
}

void MemTable::Delete(const std::string& key) {
  auto result = map->find(std::make_pair(key, std::string()));
  if (result == map->end()){
  }
  if (result != map->end()) {
    size_t estimated_size = key.size() + (*result).second.size();
    map->erase(result);
    approximate_size->fetch_sub(estimated_size, std::memory_order_relaxed);
  }
}

size_t MemTable::GetId() const {
  return id;
}

size_t MemTable::ApproximateSize() const {
  return approximate_size->load(std::memory_order_relaxed);
}