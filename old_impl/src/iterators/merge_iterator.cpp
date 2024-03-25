#include "include/merge_iterator.h"
#include "include/mem_table.h"
#include <iostream>

template class MergeIterator<MemTableIterator>;

template <typename Iterator>
MergeIterator<Iterator>::MergeIterator(std::vector<Iterator> &iterators)
{
  // Initialize the heap with the last items from each iterator
  for (size_t i = 0; i < iterators.size(); ++i)
  {
    if (iterators[i].IsValid())
    {
      heap.push({i, iterators[i]});
    }
  }
}

template <typename Iterator>
std::pair<std::string, std::string> MergeIterator<Iterator>::Next()
{
  if (heap.empty())
  {
    std::cout << "Heap is empty" << std::endl;
    // Return an empty pair if the heap is empty
    return {"", ""};
  }

  // Get the top item from the heap
  HeapWrapper<Iterator> top = heap.top();
  std::cout << "Top index: " << top.index << std::endl;
  heap.pop();

  // Get the key-value pair from the top item
  auto result = std::make_pair(top.iterator.Key(), top.iterator.Value());
  std::cout << "Key: " << result.first << " Value: " << result.second << std::endl;
  top.iterator.Next();
  std::cout << "Top index: " << top.index << std::endl;
  std::cout << "Next Key: " << top.iterator.Key() << " Next Value: " << top.iterator.Value() << std::endl;

  // Check if there are more items in the iterator associated with the popped item
  // If yes, add the next item from that iterator to the heap
  if (top.iterator.IsValid())
  {
    heap.push({top.index, top.iterator});
  }

  return result;
}

template <typename Iterator>
std::string MergeIterator<Iterator>::Key()
{
  if (heap.empty())
  {
    return "";
  }

  return heap.top().iterator.Key();
}

template <typename Iterator>
std::string MergeIterator<Iterator>::Value()
{
  if (heap.empty())
  {
    return "";
  }

  return heap.top().iterator.Value();
}

template <typename Iterator>
bool MergeIterator<Iterator>::IsValid()
{
  return !heap.empty();
}