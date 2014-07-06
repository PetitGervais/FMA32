#ifndef MEMORYALLOCTEST_H
#define MEMORYALLOCTEST_H

#include "Blocks.h"
#include "test.h"

class MemoryAllocTest : public TestBase
{
private:
  const unsigned long Alloc(MemoryBlockList & ListOfBlocks);
  const bool test(void *address, unsigned long length);
  public:
    MemoryAllocTest(const std::string testName) : TestBase(testName){}
    ~MemoryAllocTest(){}

    const bool Execute(void);

  private:
    MemoryBlockManager m_manager;
};

#endif // MEMORYALLOCTEST_H
