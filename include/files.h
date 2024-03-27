#include <string>
#include <sstream>

// Function to create a file name based on SSTable ID and level order
std::string create_file_name_based_on_level(size_t id, size_t level_order)
{
  // Construct the file name using SSTable ID and level order
  std::ostringstream oss;
  oss << "sstable_" << id << "_level_" << level_order << ".sst";
  return oss.str();
}

// Function to extract SSTable ID and level order from the file name
// Returns <SSTable ID, level order>
std::pair<size_t, size_t> extract_ids_level_from_file_name(const std::string &file_name)
{
  // Extract SSTable ID and level order from the file name
  size_t id, level_order;
  sscanf(file_name.c_str(), "sstable_%zu_level_%zu.sst", &id, &level_order);
  return std::make_pair(id, level_order);
}
