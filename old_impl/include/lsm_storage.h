#include "mem_table.h"
#include "lsm_storage_iterator.h"
#include "merge_iterator.h"
#include <shared_mutex>
#include <memory>
#include <string>
#include <optional>
#include <vector>


// Forward declaration of LsmStorageOptions
struct LsmStorageOptions;

// Forward declaration of LsmStorageState
class LsmStorageState;

// Define LsmStorageInner
class LsmStorageInner {
private:
  std::string path; // Path to the storage directory
  std::shared_ptr<LsmStorageState> state; // Pointer to the current state
  std::mutex state_lock;
  std::shared_mutex rw_lock;
  std::shared_ptr<LsmStorageOptions> options; // Pointer to the current options
  std::atomic<size_t> next_sst_id; // Atomic counter for SST IDs
public:
  LsmStorageInner(const std::string& path, const LsmStorageOptions& options);
  std::optional<std::string> Get(const std::string& key);
  void Put(const std::string& key, const std::string& value);
  void Delete(const std::string& key);
  void ForceFreezeMemTable();
  void TryFreeze(size_t size);
  size_t NextSSTId() {
    return next_sst_id.fetch_add(1, std::memory_order_relaxed);
  }
  std::shared_ptr<LsmIterator> Scan(std::string start_key, std::string end_key);
};

// Define LsmStorageOptions
struct LsmStorageOptions {
  size_t num_memtable_limit;
  size_t target_sst_size; // SST size in bytes, also the approximate memtable capacity limit
};

// Define LsmStorageState
class LsmStorageState {
private:
  std::shared_ptr<MemTable> mem_table; // Pointer to the current memtable
  std::vector<std::shared_ptr<MemTable>> imm_memtables; // Vector of immutable memtables

public:
  friend class LsmStorageInner;
  LsmStorageState() {
    mem_table = std::make_shared<MemTable>(0);
    imm_memtables = std::vector<std::shared_ptr<MemTable>>();
  }
};