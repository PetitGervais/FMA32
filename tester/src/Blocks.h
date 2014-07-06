#ifndef BLOCKS_H
#define BLOCKS_H

#include <sstream>
#include <vector>
#include "memory.h"

class MemoryBlock
{
	public:
		MemoryBlock(){}
		virtual ~MemoryBlock(){}
		void * GetAddress() { return m_address; }
		void SetAddress(void * val) { m_address = val; }
		unsigned long GetSize() { return m_size; }
		void SetSize(unsigned long val) { m_size = val; }
		unsigned long GetWantedSize() { return m_wanted_size; }
		void SetWantedSize(unsigned long val) { m_wanted_size = val; }

	private:
		void * m_address;
		unsigned long m_size;
		unsigned long m_wanted_size;
};

typedef std::vector<MemoryBlock *> MemoryBlockList;

class MemoryBlockManager
{
	public:
		MemoryBlockManager(){}
		virtual ~MemoryBlockManager(){}

		void MemoryInit(void * mem_addr, unsigned long mem_size);
		MemoryBlock * Alloc(unsigned long length);
		void Free(MemoryBlock *block);

		const bool CheckInitalMemory(void);
		bool CheckNumberOfAllocation(unsigned long nb_alloc);
		void PrintMemory(void);
		unsigned long GetMemorySize(void);
    const std::stringstream & GetError(void) const {return m_err;}

	private:
		memory_block_t * m_maa;
		void * m_mma;

    unsigned long m_maxFirstLevel;
		unsigned long m_first_level;
		unsigned long m_second_level[25];
		memory_block_t * m_fbla[25][32];
		memory_block_t m_first_block;
    std::stringstream  m_err;
};

#endif // BLOCKS_H
