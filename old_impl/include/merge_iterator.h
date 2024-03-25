#pragma once
#include <vector>
#include <queue>
#include <utility>
#include <string>
#include <algorithm>

template <typename Iterator>
struct HeapWrapper
{
  size_t index;
  Iterator iterator;
};

template <typename Iterator>
struct HeapWrapperComparator
{
  bool operator()(const HeapWrapper<Iterator> &left, const HeapWrapper<Iterator> &right) const
  {
    return left.iterator.Key() < right.iterator.Key();
  }
};

template <typename Iterator>
class MergeIterator
{
private:
  std::priority_queue<HeapWrapper<Iterator>, std::vector<HeapWrapper<Iterator>>, HeapWrapperComparator<Iterator>> heap;

public:
  MergeIterator(std::vector<Iterator> &iterators);

  std::pair<std::string, std::string> Next();
  std::string Key();
  std::string Value();
  bool IsValid();
};
