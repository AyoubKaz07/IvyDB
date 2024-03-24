#include "include/lsm_storage.h"
#include <mutex>
#include <iostream>

LsmStorageInner::LsmStorageInner(const std::string& path, const LsmStorageOptions& options) : path(path), options(std::make_shared<LsmStorageOptions>(options)) {
  // Next we pass options to construct the state
  state = std::make_shared<LsmStorageState>();
  next_sst_id = 0;
}

std::optional<std::string> LsmStorageInner::Get(const std::string& key) {
  // Lock the shared mutex for reading
  std::shared_lock<std::shared_mutex> lock(rw_lock);
  // Get the value from the memtable
  auto result = state->mem_table->Get(key);
  if (result.has_value()) {
    if (!result.value().empty()) {
      return result;
    }
    return std::nullopt;
  }
  // Iterate over the immutable memtables
  for (const auto& imm_memtable : state->imm_memtables) {
    result = imm_memtable->Get(key);
    if (result.has_value()) {
    if (!result.value().empty()) {
      return result;
    }
    return std::nullopt;
  }
  }
  return std::nullopt;
}

void LsmStorageInner::Put(const std::string& key, const std::string& value) {
  size_t size;
  {
    // Lock the shared mutex for writing
    std::shared_lock<std::shared_mutex> lock(rw_lock);
    state->mem_table->Put(key, value);
    size = state->mem_table->ApproximateSize();
  }
  TryFreeze(size);
}


// Doing dumb delete then put tombstone just for simplicity (this skiplist doesn't allow inserting twice with the same key)
void LsmStorageInner::Delete(const std::string& key) {
  size_t size;
  {
    // Lock the shared mutex for writing
    std::shared_lock<std::shared_mutex> lock(rw_lock);
    state->mem_table->Delete(key);
    state->mem_table->Put(key, "");
    size = state->mem_table->ApproximateSize();
  }

  TryFreeze(size);
}

void LsmStorageInner::TryFreeze(size_t estimated_size) {
  if (estimated_size >= options->target_sst_size) {
    state_lock.lock();
    std::shared_lock<std::shared_mutex> lock(rw_lock);
    if (state->mem_table->ApproximateSize() >= options->target_sst_size) {
      lock.unlock();
      ForceFreezeMemTable();
    } else {
      state_lock.unlock();
    }
  }
}

void LsmStorageInner::ForceFreezeMemTable() {
  size_t mem_table_id = NextSSTId();
  // Create With WAL (not implemented yet)
  std::shared_ptr<MemTable> mem_table = std::make_shared<MemTable>(mem_table_id);

  std::shared_ptr<MemTable> old_mem_table;
  {
    std::unique_lock<std::shared_mutex> lock(rw_lock);
    old_mem_table = state->mem_table;
    state->mem_table = mem_table;
    state->imm_memtables.insert(state->imm_memtables.begin(), old_mem_table);
  }
  state_lock.unlock();
}
