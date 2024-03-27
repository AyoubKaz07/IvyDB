#ifndef BLOOM_H
#define BLOOM_H

#include <boost/dynamic_bitset/dynamic_bitset.hpp>
#include <string>

class BloomFilter {
    boost::dynamic_bitset<> table;
    std::hash<std::string> hasher;

    public:
        BloomFilter(long length);

        ~BloomFilter();

        void set(const std::string& key);

        bool is_set(const std::string& key) const;

    private:
        uint64_t string_to_uint64(const std::string& key) const;

        uint64_t hash_1(uint64_t key) const;

        uint64_t hash_2(uint64_t key) const;

        uint64_t hash_3(uint64_t key) const;
};

#endif //BLOOM_H