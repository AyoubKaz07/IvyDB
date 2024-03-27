#include "include/bloom.h"

/**
 * Implementation of a Bloom filter using k = 3 hash functions.
 * The Hash functions are taken from https://gist.github.com/badboy/6267743
 */


BloomFilter::BloomFilter(long length): table(length) {}

BloomFilter::~BloomFilter() {
    ~table;
}

void BloomFilter::set(const std::string& key) {
    uint64_t k = string_to_uint64(key);

    table.set(hash_1(k));
    table.set(hash_2(k));
    table.set(hash_3(k));
}

bool BloomFilter::is_set(const std::string& key) const {
    uint64_t k = string_to_uint64(key);

    return table.test(hash_1(k))
        && table.test(hash_2(k)) 
        && table.test(hash_3(k));
}

uint64_t BloomFilter::string_to_uint64(const std::string& key) const {
    return hasher(key);
}

uint64_t BloomFilter::hash_1(uint64_t key) const {
    key = ~key + (key<<15);
    key = key ^ (key>>12);
    key = key + (key<<2);
    key = key ^ (key>>4);
    key = key * 2057;
    key = key ^ (key>>16);

    return key % table.size();
}

uint64_t BloomFilter::hash_2(uint64_t key) const {
    key = (key+0x7ed55d16) + (key<<12);
    key = (key^0xc761c23c) ^ (key>>19);
    key = (key+0x165667b1) + (key<<5);
    key = (key+0xd3a2646c) ^ (key<<9);
    key = (key+0xfd7046c5) + (key<<3);
    key = (key^0xb55a4f09) ^ (key>>16);

    return key % table.size();
}

uint64_t BloomFilter::hash_3(uint64_t key) const {
    key = (key^61) ^ (key>>16);
    key = key + (key<<3);
    key = key ^ (key>>4);
    key = key * 0x27d4eb2d;
    key = key ^ (key>>15);

    return key % table.size();
}