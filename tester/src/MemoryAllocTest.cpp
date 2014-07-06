#include "MemoryAllocTest.h"
#include "bitwise.h"

struct MemoryInformation {
  unsigned long MemSize;
  unsigned long Iteration;
  unsigned long MaxAllocSize;
 };

const struct MemoryInformation MemInfo[4] = {{        8 * 1024,    1000000,      512 + 1},
                                             {      128 * 1024,     100000,     1024 + 1},
                                             { 1 * 1024 * 1024,     100000,    16384 + 1},
                                             {16 * 1024 * 1024,      10000,    32768 + 1}};
                                    
const bool MemoryAllocTest::test(void *address, unsigned long length)
{
  srand(time(NULL));
  
  for(unsigned MemIter = 0; MemIter < 4; MemIter++)
  {
    MemoryBlockList ListOfBlocks;
    
    // Initialize the memory
    std::cout << "Test for a memory size of " << MemInfo[MemIter].MemSize << " bytes :" << std::endl;
	  m_manager.MemoryInit(address, MemInfo[MemIter].MemSize);
    unsigned long NbAlloc = 0;
    unsigned long NbFree = 0;
    
    // Allocation & Free
    for(unsigned long Counter = 0; Counter < MemInfo[MemIter].Iteration; Counter++)
    {
      // Allocate memory
      MemoryBlock * CurrentBlock;
      do
      {
        // Try to fill the memory
        unsigned long Length = rand() % MemInfo[MemIter].MaxAllocSize;
        CurrentBlock = m_manager.Alloc(Length);
        if(CurrentBlock != nullptr)
        {
          NbAlloc++;
          ListOfBlocks.push_back(CurrentBlock);
        }
      }while(CurrentBlock != nullptr);
      
      // Free half number of blocks
      for(unsigned long FreeCounter = 0; FreeCounter < (ListOfBlocks.size() / 2); FreeCounter++)
      {
        MemoryBlockList::iterator iter = ListOfBlocks.begin() + (rand() % ListOfBlocks.size());
        m_manager.Free(*iter);
        NbFree++;
        ListOfBlocks.erase(iter);
      }
    }
    
    // Free the rest of allocated block
    for(MemoryBlockList::iterator iter = ListOfBlocks.begin(); iter != ListOfBlocks.end(); iter++)
    {
      m_manager.Free(*iter);
      NbFree++;
    }
    
    // Check the memory integrity
    if(m_manager.CheckInitalMemory() == false)
    {
      std::stringstream & err = GetError();
      err << m_manager.GetError().str();
      return false;
    }
    std::cout <<  "Number of allocation : " << NbAlloc << "    Number of free : " << NbFree << std::endl;
  }
  return true;
}

const bool MemoryAllocTest::Execute(void)
{
  std::cout << "*******************************" << std::endl;
  std::cout << "* " << this->GetName() << std::endl;
  std::cout << "*******************************" << std::endl;

  char * address = new char[MemInfo[3].MemSize];

  bool TestPass = test(address, MemInfo[3].MemSize);
  delete [] address;
  return TestPass;
}
