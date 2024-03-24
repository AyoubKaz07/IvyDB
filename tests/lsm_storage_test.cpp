#include <gtest/gtest.h>
#include <thread>
#include "include/lsm_storage.h"


// BAD TESTS BECAUSE SKIPLIST ISN'T CONCURRENT ?

TEST(LsmStorageTest, TestPutGet) {
  LsmStorageOptions options = {1, 1024};
  LsmStorageInner storage("test", options);
  storage.Put("key", "value");
  auto result = storage.Get("key");
  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(result.value(), "value");
}

TEST(LsmStorageTest, TestPutGetMultiple) {
  LsmStorageOptions options = {1, 1024};
  LsmStorageInner storage("test", options);
  for (int i = 0; i < 100; i++) {
    storage.Put(std::to_string(i), std::to_string(i));
  }
  for (int i = 0; i < 100; i++) {
    auto result = storage.Get(std::to_string(i));
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), std::to_string(i));
  }
}

TEST(LsmStorageTest, TestDelete) {
  LsmStorageOptions options = {1, 1024};
  LsmStorageInner storage("test", options);
  storage.Put("key", "value");
  storage.Delete("key");
  auto result = storage.Get("key");
  ASSERT_FALSE(result.has_value());
}

TEST(LsmStorageTest, TestDeleteMultiple) {
  LsmStorageOptions options = {1, 1024};
  LsmStorageInner storage("test", options);
  for (int i = 0; i < 100; i++) {
    storage.Put(std::to_string(i), std::to_string(i));
  }
  for (int i = 0; i < 100; i++) {
    storage.Delete(std::to_string(i));
    auto result = storage.Get(std::to_string(i));
    ASSERT_FALSE(result.has_value());
  }
}

TEST(LsmStorageTest, TestPutGetDelete) {
  LsmStorageOptions options = {1, 1024};
  LsmStorageInner storage("test", options);
  storage.Put("key", "value");
  auto result = storage.Get("key");
  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(result.value(), "value");
  storage.Delete("key");
  result = storage.Get("key");
  ASSERT_FALSE(result.has_value());
}

// Small size of memtable to trigger freeze
TEST(LsmStorageTest, TestFreeze) {
  LsmStorageOptions options = {1, 1};
  LsmStorageInner storage("test", options);
  for (int i = 0; i < 100; i++) {
    storage.Put(std::to_string(i), std::to_string(i));
  }
  for (int i = 0; i < 100; i++) {
    auto result = storage.Get(std::to_string(i));
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), std::to_string(i));
  }
}

// multithreaded test for freezing
TEST(LsmStorageTest, TestFreezeMultithreaded) {
  LsmStorageOptions options = {1, 50};
  LsmStorageInner storage("test", options);
  std::vector<std::thread> threads;
  for (int i = 0; i < 100; i++) {
    threads.push_back(std::thread([&storage, i](){
      storage.Put(std::to_string(i), std::to_string(i));
    }));
  }
  for (auto& thread : threads) {
    thread.join();
  }
  for (int i = 0; i < 100; i++) {
    auto result = storage.Get(std::to_string(i));
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), std::to_string(i));
  }
}


TEST(LsmStorageTest, TestPutGetDeleteMultithreaded) {
  LsmStorageOptions options = {1, 20};
  LsmStorageInner storage("test", options);
  std::vector<std::thread> threads;
  for (int i = 0; i < 100; i++) {
    threads.push_back(std::thread([&storage, i](){
      storage.Put(std::to_string(i), std::to_string(i));
    }));
  }
  for (int i = 0; i < 100; i++) {
    threads.push_back(std::thread([&storage, i](){
      auto result = storage.Get(std::to_string(i));
      std::cout << "Get " << i << std::endl;
      ASSERT_TRUE(result.has_value());
      ASSERT_EQ(result.value(), std::to_string(i));
    }));
  }
  for (int i = 0; i < 100; i++) {
    threads.push_back(std::thread([&storage, i](){
      storage.Delete(std::to_string(i));
    }));
  }
  for (auto& thread : threads) {
    thread.join();
  }
  // for (int i = 0; i < 100; i++) {
  //   storage.Delete(std::to_string(i));
  // }
  for (int i = 0; i < 100; i++) {
    auto result = storage.Get(std::to_string(i));
    ASSERT_FALSE(result.has_value());
  }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}