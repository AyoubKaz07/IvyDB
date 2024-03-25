#pragma once
#include "include/merge_iterator.h"
#include "include/mem_table.h"

class LsmIterator
{
private:
  MergeIterator<MemTableIterator> inner;

public:
  LsmIterator(MergeIterator<MemTableIterator> iter) : inner(iter) {};

  void move_to_non_delete();

  bool IsValid();

  const std::string Key();

  const std::string Value();

  void Next();
};
