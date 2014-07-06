#include "Blocks.h"
#include <iostream>
#include <cstring>
#include "bitwise.h"

extern memory_management_area_t mma;

MemoryBlock * MemoryBlockManager::Alloc(unsigned long length)
{
	memory_block_t * real_block = (memory_block_t *)memory_alloc(length);
	if(real_block == nullptr)
    return nullptr;

	MemoryBlock * block = new MemoryBlock;
	if(real_block != nullptr)
		 block->SetAddress((void *)((unsigned long) real_block - BLOCK_HEADER_SIZE_USED));

	block->SetWantedSize(length);
	(real_block == nullptr) ? block->SetSize(0) : block->SetSize(BLOCK_GET_MASKED_SIZE((memory_block_t *)block->GetAddress()));

	memset((char *)real_block, 0xAA, block->GetSize());
	return block;
}

void MemoryBlockManager::Free(MemoryBlock *block)
{
	void * ptr = (void *)((unsigned long)block->GetAddress() + BLOCK_HEADER_SIZE_USED);
	memory_free(ptr);
	delete block;
}

void MemoryBlockManager::MemoryInit(void * mem_addr, unsigned long mem_size)
{
	memory_init(mem_addr, mem_size);

	m_mma = (void *)mma.first_level;

	// Build the footprint of the initial memory state
  unsigned long mma_area_size;
  m_maxFirstLevel = bit_highest_pos(bit_next_power_of_two((unsigned long)mem_size)) - 6;

	/* Set the first level in the MMA */
	m_first_level = *(unsigned long *)m_mma;
	mma_area_size = 4;

	/* Set the second level in the MMA */
	unsigned long *ptr_fl = (unsigned long *)((unsigned long)mem_addr + mma_area_size);
	for(unsigned counter = 0; counter < m_maxFirstLevel; counter++)
    m_second_level[counter] = ptr_fl[counter];
	mma_area_size += m_maxFirstLevel * 4;

	/* Set the fbla */
	memory_block_t *(*fbla)[32] = (memory_block_t *(*)[32])((unsigned long)mem_addr + mma_area_size);
	for(unsigned long fl = 0; fl < m_maxFirstLevel; fl++)
    for(unsigned long sl = 0; sl < 32; sl++)
      m_fbla[fl][sl] = fbla[fl][sl];

	mma_area_size += m_maxFirstLevel * (32 * 4);
  m_maa = (memory_block_t *)((unsigned long)mem_addr + mma_area_size);
  m_first_block.size = m_maa->size;
  m_first_block.phys_prev = m_maa->phys_prev;
  m_first_block.next = m_maa->next;
  m_first_block.prev = m_maa->prev;
}

void MemoryBlockManager::PrintMemory(void)
{
  unsigned long sizeBlock;
	memory_block_t  *block = m_maa;
  std::cout << "**** Start scanning memory *****\n";
  while(1)
  {
    if(BLOCK_IS_LAST(block))
      break;
    sizeBlock = BLOCK_GET_MASKED_SIZE(block);
    std::cout << "Block " << block << " " << ((BLOCK_IS_FREE(block)) ? "F":"U") << " size: " << sizeBlock << std::endl;
    block = (memory_block_t *) ((unsigned long)block + sizeBlock + BLOCK_HEADER_SIZE_USED);
  }
  sizeBlock = BLOCK_GET_MASKED_SIZE(block);
  std::cout << "Block " << block << " " << ((BLOCK_IS_FREE(block)) ? "F":"U") << " size: " << sizeBlock << std::endl;
  std::cout << "**** End scanning memory *****\n";
}

const bool MemoryBlockManager::CheckInitalMemory(void)
{
  memory_block_t * block = m_maa;

	if(m_first_block.size != block->size)
  {
    m_err << "Memory integrity error on size : expected " << m_first_block.size << " has " << block->size;
    return false;
  }

  if(m_first_block.phys_prev != block->phys_prev)
  {
    m_err << "Memory integrity error on phys_prev pointer: expected " << m_first_block.phys_prev << " has " << block->phys_prev;
    return false;
  }

  if(m_first_block.next != block->next)
  {
    m_err << "Memory integrity error on next pointer : expected " << m_first_block.next << " has " << block->next;
    return false;
  }

  if(m_first_block.prev != block->prev)
  {
    m_err << "Memory integrity error on prev pointer : expected " << m_first_block.prev << " has " << block->prev;
    return false;
  }

  /* Set the first level in the MMA */
	if(m_first_level != *(unsigned long *)(m_mma))
  {
    m_err << "Memory integrity error on FL : expected " << m_first_level << " has " <<*(unsigned long *)(m_mma);
    return false;
  }

	unsigned long mma_area_size = 4;
	unsigned long *ptr_fl = (unsigned long *)((unsigned long)m_mma + mma_area_size);
	for(unsigned counter = 0; counter < m_maxFirstLevel; counter++)
  {
    if(m_second_level[counter] != ptr_fl[counter])
    {
      m_err << "Memory integrity error on SL : expected " << m_second_level[counter] << " has " << ptr_fl[counter];
      return false;
    }
  }
	mma_area_size += m_maxFirstLevel * 4;

	memory_block_t *(*fbla)[32] = (memory_block_t *(*)[32])((unsigned long)m_mma + mma_area_size);
	for(unsigned long fl = 0; fl < m_maxFirstLevel; fl++)
  {

    for(unsigned long sl = 0; sl < 32; sl++)
    {
      if(m_fbla[fl][sl] != fbla[fl][sl])
      {
        m_err << "Memory integrity error on SL : expected " << m_fbla[fl][sl] << " has " << fbla[fl][sl];
        return false;
      }
    }
  }
  return true;
}

