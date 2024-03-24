#include <atomic>
#include <map>
#include <utility>
#include <vector>
#include <cstdint>
#include <string>
#include <optional>
#include <memory>
#include "include/skip_list.h"



// Define comparison function for keys
struct KeyCompare {
  bool operator()(const std::pair<std::string, std::string>& lhs, const std::pair<std::string, std::string>& rhs) const {
    return lhs.first < rhs.first;
  }
};


class MemTable {
private:
  // THIS IS NOT CONCURRENT 
  std::shared_ptr<goodliffe::skip_list<std::pair<std::string, std::string>, KeyCompare>> map;
  size_t id;
  std::shared_ptr<std::atomic<size_t>> approximate_size;

public:
  MemTable(size_t id);
  std::optional<std::string> Get(const std::string& key);
  void Put(const std::string& key, const std::string& value);
  void Delete(const std::string& key);
  size_t GetId() const;
  size_t ApproximateSize() const;
};