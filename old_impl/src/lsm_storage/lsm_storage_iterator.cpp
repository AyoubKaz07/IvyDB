#include "include/lsm_storage_iterator.h"
#include "include/merge_iterator.h"


void LsmIterator::move_to_non_delete()
{
  while (IsValid() && inner.Value().empty())
  {
    inner.Next();
  }
}

bool LsmIterator::IsValid()
{
  return inner.IsValid();
}

const std::string LsmIterator::Key() 
{
  return inner.Key();
}

const std::string LsmIterator::Value() 
{
  return inner.Value();
}

void LsmIterator::Next()
{
  inner.Next();
  move_to_non_delete();
}