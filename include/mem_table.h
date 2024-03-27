#pragma once
#include <string>
#include <map>
#include <atomic>
#include <optional>
#include <vector>
#include <algorithm>
#include <iostream>
#include <memory>

// struct NumericStringComparator
// {
//   bool operator()(const std::string &a, const std::string &b) const
//   {
//     // Remove non-numeric characters from strings
//     auto stripNonNumeric = [](const std::string &s)
//     {
//       std::string result;
//       std::copy_if(s.begin(), s.end(), std::back_inserter(result), ::isdigit);
//       return result;
//     };

//     // Convert strings to integers and compare numerically
//     return std::stoi(stripNonNumeric(a)) < std::stoi(stripNonNumeric(b));
//   }
// };

// struct CustomStringComparator
// {
//   bool operator()(const std::string &lhs, const std::string &rhs) const
//   {
//     // Convert strings to lowercase for comparison
//     std::string lower_lhs = toLower(lhs);
//     std::string lower_rhs = toLower(rhs);

//     // Compare strings strictly in lowercase
//     return lower_lhs < lower_rhs;
//   }
// };

class MemTable
{
public:
  MemTable() = default;
  ~MemTable() = default;
  void Put(const std::string &key, const std::string &value)
  {
    map[key] = value;
    // for (auto it = map.begin(); it != map.end(); ++it)
    // {
    //   std::cout << it->first << " " << it->second << std::endl;
    // }
    approximate_size->fetch_add(key.size() + value.size(), std::memory_order_relaxed);
  }
  std::optional<std::string> Get(const std::string &key)
  {
    auto it = map.find(key);
    if (it != map.end())
    {
      return it->second;
    }
    return std::nullopt;
  }
  // void Delete(const std::string &key)
  // {
  //   // Tombstone delete
  //   auto it = map.find(key);
  //   if (it != map.end())
  //   {
  //     map.erase(it);
  //     approximate_size->fetch_sub(key.size() + it->second.size());
  //   }
  // }

  size_t ApproximateSize() const
  {
    return approximate_size->load(std::memory_order_relaxed);
  }

  // Return all kv pairs in the memtable and clear it
  std::vector<std::pair<std::string, std::string>> GetAndClear()
  {
    std::vector<std::pair<std::string, std::string>> result;
    for (auto it = map.begin(); it != map.end(); ++it)
    {
      result.push_back({it->first, it->second});
    }

    map.clear();
    approximate_size->store(0, std::memory_order_relaxed);
    // for (auto r : result)
    // {
    //   std::cout << r.first << " " << r.second << std::endl;
    // }
    return result;
  }

  size_t Size() const
  {
    return map.size();
  }

  void Clear()
  {
    map.clear();
    approximate_size->store(0, std::memory_order_relaxed);
  }

  // Function to find the key in the map that is nearest to the target key
  std::optional<std::string> FindNearest(const std::string &target)
  {
    auto it = map.lower_bound(target); // Find the first element with a key not less than the target key
    if (it == map.begin())
    {             // If the target key is less than the first key in the map
      return std::nullopt; // Return the first key
    }
    else if (it == map.end())
    {               // If the target key is greater than or equal to the last key in the map
      return (*--it).second; // Return the last key
    }
    else
    {
      auto prev = std::prev(it); // Get the iterator to the previous key
      return (*prev).second;
    }
  }

private:
  // std::map<std::string, std::string, NumericStringComparator> map;
  std::map<std::string, std::string> map;
  std::shared_ptr<std::atomic<size_t>> approximate_size = std::make_shared<std::atomic<size_t>>(0);
};