#include <string>
#include "mem_table.h"
#include "bloom.h"

#define SPARSITY_FACTOR 10

class SSTable {
  public:
    /**
     * @brief Construct a new SSTable object from a flushed MemTable.
     * @param path The path to the SSTable file.
     * @param id The ID of the SSTable.
     * @param memtable The MemTable to flush to the SSTable.
    */
    SSTable(const std::string &path, const size_t &id, MemTable &memtable, long size);

    /**
     * @brief Construct a new SSTable object from two SSTables.
     * @param path The path to the SSTable file.
     * @param id The ID of the SSTable.
     * @param sst_1 The first SSTable to merge.
     * @param sst_2 The second SSTable to merge.
    */
    SSTable(const std::string &path, const size_t &id, SSTable* sst_1, SSTable* sst_2, long size);

    /**
     * @brief Construct a new SSTable object from an existing SSTable file (used for recovery).
    */
    SSTable(const std::string &path, const size_t &id, long size);

    /**
     * @brief Destroy the SSTable object. (just removes the file from disk)
    */
    ~SSTable();

    /**
     * @brief rebuilds the sparse index and bloom filter from the SSTable file. (used in above constructor of recovery)
     * @param path The path to the SSTable file.
    */
    void RepopulateInternals(const std::string &path);

    /**
     * @brief Get the value of a key from the SSTable.
     * @param key The key to get the value for.
     * @return The value of the key.
    */
    std::string Get(const std::string &key);

    /**
     * @brief Get the path of the SSTable.
     * @return The path of the SSTable.
    */
    std::string GetPath() const;

    /**
     * @brief Clear the SSTable file.
    */
    void Clear();
  private:
    std::string path;
    MemTable sparse_index;
    BloomFilter bloom_filter;
    size_t id = 0;
};