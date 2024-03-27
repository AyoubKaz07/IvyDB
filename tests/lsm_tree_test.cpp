#include <gtest/gtest.h>
#include <random>
#include <string>
#include "include/lsm_tree.h"


std::string generateRandomString(int length)
{
  const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  std::random_device rd;
  std::mt19937 generator(rd());
  std::uniform_int_distribution<int> distribution(0, characters.size() - 1);

  std::string randomString;
  for (int i = 0; i < length; ++i)
  {
    randomString += characters[distribution(generator)];
  }
  return randomString;
}




TEST(LSMTreeTest, SimplePutGet)
{
  LSMTree lsm_tree;
  lsm_tree.Put("key1", "value1");
  EXPECT_EQ(lsm_tree.Get("key1"), "value1");
  lsm_tree.DropDB();
}

TEST(LSMTreeTest, SimplePutDelete)
{
  LSMTree lsm_tree;
  lsm_tree.Put("key1", "value1");
  lsm_tree.Delete("key1");
  EXPECT_EQ(lsm_tree.Get("key1"), "");
  lsm_tree.DropDB();
}

// break LSMTreeTest_MultipleOperations_Test::TestBody()
TEST(LSMTreeTest, MultipleOperations)
{
  LSMTree lsm_tree;
  for (int i = 0; i < 100; i++)
  {
    std::string key = generateRandomString(10);
    std::string value = generateRandomString(10);
    lsm_tree.Put(key, value);
  }
  for (int i = 0; i < 100; i++)
  {
    std::string key = generateRandomString(10);
    std::string value = generateRandomString(10);
    lsm_tree.Put(key, value);
    EXPECT_EQ(lsm_tree.Get(key), value);
  }
  for (int i = 0; i < 100; i++)
  {
    std::string key = generateRandomString(10);
    lsm_tree.Delete(key);
  }
  for (int i = 0; i < 100; i++)
  {
    std::string key = generateRandomString(10);
    EXPECT_EQ(lsm_tree.Get(key), "");
  }
  lsm_tree.DropDB();
}

// Recovery test
// break LSMTreeTest_Recovery_Test::TestBody()
std::vector<std::pair<std::string, std::string>> old_fail_kv_pairs = {
    {"DhDalDHqRt", "DaSWZTZPeZ"},
    {"DmiahJzEvl", "lDkWWCgfoY"},
    {"DpZXLPcTGs", "HhaVPRpaUY"},
    {"EsIlCYOgZe", "KNjkGWIPfz"},
    {"GqwfRouIOd", "vQSHLNHDiR"},
    {"GxupNQEXJi", "SbFtREhMLt"},
    {"HRdIOHMxGf", "fsHduUnHpK"},
    {"HqLqhDjvJw", "xEsTSPWPso"},
    {"JjBxPqqClv", "mXnrApvSbC"},
    {"KWnObrpNGm", "nCzySKffzW"},
    {"LTBcJPgPbv", "hioKujHuQT"},
    {"LoImKIOvHf", "tDKBGsjpPv"},
    {"vfgNKUCdPa", "aJfQpSarNu"},
    {"xXhGhQcYfZ", "PEmfQoCxGD"},
    {"zueSJnJEpg", "NAQyMJsLAF"},
    {"zzzHrYHfXY", "HBddPxEVbA"}};

std::vector<std::pair<std::string, std::string>> wal_kv_pairs = {
    {"Ayoub", "Wissal"}};

TEST(LSMTreeTest, Recovery)
{
  LSMTree lsm_tree;

  // Define paths
  const std::string wal_path = "../storage/wal.log";
  const std::string sstables_path = "../storage/sstables/";

  // Create recovery files
  for (int level_id = 0; level_id < 1; ++level_id)
  {
    for (int id = 0; id < 1; ++id)
    {
      std::string sstable_file_path = sstables_path + "sstable_" + std::to_string(id) + "_level_" + std::to_string(level_id) + ".sst";
      std::ofstream sstable_file(sstable_file_path);
      for (const auto &pair : old_fail_kv_pairs)
      {
        sstable_file << pair.first << "," << pair.second << std::endl;
      }
      sstable_file.close();
    }
  }

  // Create WAL file
  std::ofstream wal_file(wal_path);
  for (const auto &pair : wal_kv_pairs)
  {
    wal_file << pair.first << "," << pair.second << std::endl;
  }

  // Simulate recovery
  lsm_tree.Recover();

  // Test against inserted values and data from recovery files
  for (const auto &pair : old_fail_kv_pairs)
  {
    EXPECT_EQ(lsm_tree.Get(pair.first), pair.second);
  }
  for (const auto &pair : wal_kv_pairs)
  {
    EXPECT_EQ(lsm_tree.Get(pair.first), pair.second);
  }
  lsm_tree.DropDB();
}