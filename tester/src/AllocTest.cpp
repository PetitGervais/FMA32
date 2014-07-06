#include "MemoryAllocTest.h"

int main()
{
  Tester test;

  // Check allocation / free tests
  test.Register(new MemoryAllocTest("Alloc/Free tests"));

  test.Run();
  return 0;
}
