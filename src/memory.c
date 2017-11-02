/*  This file is part of FMA32
    Fast Memory Allocator for 32 bits embedded system.
    (Romain CARITEY - 2014)

    FMA32 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FMA32 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FMA32.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "bitwise.h"
#include "memory.h"

STATIC memory_management_area_t mma;

/******************************************************************************
 * block_get_levels
 * Get the level according to the size
 *
 * [in] size    : wanted size
 * [out] level  : level initialized according to the size
 *
 *****************************************************************************/
STATIC void block_get_levels(unsigned long size, memory_level_t *level)
{
  size = RESIZE_DOWN(size, 4);

  /* Calculate fl and sl values */
  if(size < 128)
  {

    level->fl = 0;
    level->fl_bitmap = 1;
    level->sl = size >> 2;
    level->sl_bitmap = 1 << level->sl;
  }
  else
  {
    level->fl = bit_highest_pos(size) - 6;
    level->fl_bitmap = 1 << level->fl;
    level->sl = ((size - ((level->fl_bitmap << 6))) / (level->fl_bitmap >> 1)) >> 2;
    level->sl_bitmap = 1 << level->sl;
  }
}

/******************************************************************************
 * block_get_next_level
 * Get the next level according to the size
 *
 * [in] size    : wanted size
 * [out] level  : next level initialized according to the size
 *****************************************************************************/
STATIC unsigned long block_get_next_level(unsigned long size, memory_level_t *level)
{
  block_get_levels(size, level);
  level->sl_bitmap <<= 1;
  if(level->sl_bitmap == 0)
  {
    level->sl = 0;
    level->fl++;
    level->fl_bitmap <<= 1;
    if(level->fl_bitmap == 0)
      return 0;
  }
  else
  {
    level->sl++;
  }
  return 1;
}

/******************************************************************************
 * block_insert
 * Insert a block at the head of the free block list
 *
 * [in] block : block to insert
 *****************************************************************************/
STATIC void block_insert(memory_block_t * block)
{
  memory_level_t level;

  /* Update  bitmaps */
  block_get_levels(block->size, &level);
  *mma.first_level |= level.fl_bitmap;
  mma.second_level[level.fl] |= level.sl_bitmap;

  memory_block_t * free_blocks_ptr = mma.fbla[level.fl][level.sl];
  if(free_blocks_ptr == NULL)
  {
    /* No next block */
    block->next = NULL;
  }
  else
  {
    /* Update the next block */
    block->next = free_blocks_ptr;
    free_blocks_ptr->prev = (struct memory_block_s *)block;
  }
  block->prev = NULL;
  mma.fbla[level.fl][level.sl] = block;
  BLOCK_MARK_AS_FREE(block);
}

/******************************************************************************
 * block_extract
 * Extract a block from the free list
 *
 * [in] block : block to extract
 *****************************************************************************/
STATIC void block_extract(memory_block_t * block)
{
  memory_level_t level;

  if(block->prev == NULL)
  {
    block_get_levels(block->size, &level);
    /* It is the first block in the list */
    if(block->next == NULL)
    {
      /* Block is alone in the list, update the list and bitmap level */
      /* Set the list as empty */
      mma.fbla[level.fl][level.sl] = NULL;

      /* Adjust second level bitmap */
      mma.second_level[level.fl] = bit_clear(mma.second_level[level.fl], level.sl);
      /* Adjust first level bitmap if needed */
      if(mma.second_level[level.fl] == 0)
        *mma.first_level = bit_clear(*mma.first_level, level.fl);
    }
    else
    {
      /* Set next block to be the first block */
      block->next->prev = NULL;
      /* Set the next block as the head of the list */
      mma.fbla[level.fl][level.sl] = block->next;
    }
  }
  else if(block->next == NULL)
  {
    /* It is the last block in the list */
    block->prev->next = NULL;
  }
  else
  {
    /* Block is between two existing block */
    block->next->prev = block->prev;
    block->prev->next = block->next;
  }

  /* Reset list pointer of the extracted block */
  block->next = NULL;
  block->prev = NULL;
}

/******************************************************************************
 * block_find
 * Find a free block in the level
 *
 * [in] level : level to start the searching of a free block
 *
 * Return null if no free block found else address of free block
 *****************************************************************************/
STATIC memory_block_t * block_find(memory_level_t *level)
{
  /* Check if we can found a block in the current level */
  unsigned long bitmap = mma.second_level[level->fl] & level->sl_bitmap;

  if(bitmap == 0)
  {
    /* Not found, check if we can found a greater block in the current second level */
    bitmap = BLOCK_MASK_FREE(level->sl_bitmap);
    bitmap &= mma.second_level[level->fl];
    if(bitmap == 0)
    {
      /* Not found, check if we can found a block in the next first layer */
      bitmap = BLOCK_MASK_FREE(level->fl_bitmap << 1);
      bitmap &= *mma.first_level;
      if(bitmap == 0)
        return NULL; /* No free block available */
      else
      {
        /* Find free block in a greater first level */
        /* Get the first free block */
        level->fl = bit_lowest_pos(bitmap);
        level->fl_bitmap = 1 << level->fl;
        level->sl = bit_lowest_pos(mma.second_level[level->fl]);
        level->sl_bitmap = 1 << level->sl;
      }
    }
    else
    {
      /* Found free greater block in the next second level */
      level->sl = bit_lowest_pos(bitmap);
      level->sl_bitmap = 1 << level->sl;
    }
  }
  return mma.fbla[level->fl][level->sl];
}

/******************************************************************************
 * block_get_physical_next
 * Get the next physical block from the current block
 *
 * [in] block : current block
 *****************************************************************************/
STATIC inline memory_block_t * block_get_physical_next(memory_block_t *block)
{
  return (memory_block_t *)((unsigned long) block + BLOCK_GET_MASKED_SIZE(block) + BLOCK_HEADER_SIZE_USED);
}

/******************************************************************************
 * block_split
 * Split a block
 *
 * [in] block : block to split
 * [in] size  : size to split the block
 *****************************************************************************/
STATIC void block_split(memory_block_t *block, unsigned long size)
{
  memory_block_t * new_free_block;

  /* Compute a temporary estimated size */
  unsigned long tmp_size = BLOCK_GET_MASKED_SIZE(block) - size;

  /* Check for splitting the block */
  if(tmp_size >= BLOCK_MIN_SIZE)
  {
    /* Split the block and create the new free block */
    new_free_block = (memory_block_t *)((unsigned long)block + size + BLOCK_HEADER_SIZE_USED);
    new_free_block->size = tmp_size - BLOCK_HEADER_SIZE_USED;
    new_free_block->next = NULL;
    new_free_block->prev = NULL;
    new_free_block->phys_prev = block;

    /* Update block size */
    block->size = size | BLOCK_GET_FLAG_BIT(block);

    if(BLOCK_IS_LAST(block))
    {
      /* Mark the block as not last block */
      BLOCK_MARK_AS_NOT_LAST(block);
      /* Mark new free block as last */
      BLOCK_MARK_AS_LAST(new_free_block);
    }
    else
    {
      /* Update previous pointer of the next block of the new free block */
      block_get_physical_next(new_free_block)->phys_prev = new_free_block;
    }

    /* Mark blocks */
    BLOCK_MARK_AS_USED(block);
    BLOCK_MARK_AS_FREE(new_free_block);

    /* Insert new free block in the chain list*/
    block_insert(new_free_block);
  }
  else
  {
    /* Can't split, keep the current size */
    BLOCK_MARK_AS_USED(block);
  }
}

/******************************************************************************
 * block_merge_right
 * Merge the current with the physical right block
 *
 * [in] current_block : Current block to merge
 * [in] right_block   : Right block to merge with the current
 *****************************************************************************/
STATIC memory_block_t * block_merge_right(memory_block_t * current_block)
{
  memory_block_t * right_block;

  /* Check for right merge, if the current block is the last, no needs to merge right */
  if(!BLOCK_IS_LAST(current_block))
  {
    /* Check if the next block is free */
    right_block = block_get_physical_next(current_block);
    if(BLOCK_IS_FREE(right_block))
    {
      /* Right merge */
      if(BLOCK_IS_LAST(right_block))
      {
        /* Mark now the current block as the last block */
        BLOCK_MARK_AS_LAST(current_block);
      }
      else
      {
        /* Here right_block is not the last block so update
        physical previous pointer of the next block */
        block_get_physical_next(right_block)->phys_prev = current_block;
      }
      block_extract(right_block);
      current_block->size += BLOCK_GET_MASKED_SIZE(right_block) + BLOCK_HEADER_SIZE_USED;
    }
  }
  return current_block;
}

/******************************************************************************
 * block_merge_left
 * Merge the current with the physical left block
 *
 * [in] current_block : Current block to merge
 * [in] left_block    : Left block to merge with the current
 *****************************************************************************/
STATIC memory_block_t * block_merge_left(memory_block_t * current_block)
{
  memory_block_t * left_block = current_block->phys_prev;

  /* If left block is null means block is the first physical block, so no left merge */
  if(left_block != NULL)
  {
    /* Check for left merge */
    if(BLOCK_IS_FREE(left_block))
    {
      if(BLOCK_IS_LAST(current_block))
      {
        BLOCK_MARK_AS_LAST(left_block);
      }
      else
      {
        /* block is not the last block so update physical previous
        pointer of the next block */
        block_get_physical_next(current_block)->phys_prev = left_block;
      }
      block_extract(left_block);
      left_block->size += BLOCK_GET_MASKED_SIZE(current_block) + BLOCK_HEADER_SIZE_USED;
      return left_block;
    }
  }
  return  current_block;
}


/******************************************************************************
 * memory_init
 * Memory initialization. Prepare the memory structure
 *
 * [in] mem_ptr : memory pointer
 * [in] length  : size of the memory
 *****************************************************************************/
unsigned long memory_init(void * address, unsigned long length)
{
  memory_block_t * first_block;
  unsigned reset_size;
  unsigned long mma_area_size;
  unsigned long level_max = bit_highest_pos(bit_next_power_of_two((unsigned long)length)) - 6;

  /* Align memory header address and size to be modulo 32 bits */
  if((unsigned long)address & ALIGN_MASK)
  {
    /* address is not aligned */
    address = (void *)RESIZE_UP(address, LONG_SIZE_BYTE);
    length -= LONG_SIZE_BYTE;
  }
  else
  {
    length = RESIZE_DOWN(length, LONG_SIZE_BYTE);
  }

  /* Set the first level in the MMA */
  mma.first_level = (unsigned long *)address;
  mma_area_size = 4;

  /* Set the second level in the MMA */
  mma.second_level = (unsigned long *)((unsigned long)address + mma_area_size);
  mma_area_size += level_max * 4;

  /* Set the second level in the MMA */
  mma.fbla = (memory_block_t *(*)[32])((unsigned long)address + mma_area_size);
  mma_area_size += level_max * (32 * 4);

  /* Check the size */
  if((mma_area_size + BLOCK_MIN_SIZE) > length)
    return 0;

  /* Reset the MMA area */
  for(reset_size = 0; reset_size < mma_area_size; reset_size++)
    mma.first_level[reset_size] = 0;

  /* Set the first free block in memory block area */
  first_block = (memory_block_t *)((unsigned long)address + mma_area_size);

  /* Set the size of the fisrt block */
  first_block->size = length - mma_area_size - BLOCK_HEADER_SIZE_USED;

  /* Put in place the first block in the chain list */
  block_insert(first_block);

  /* No previous physical block */
  first_block->phys_prev = NULL;

  /* Mark the block as last */
  BLOCK_MARK_AS_LAST(first_block);

  return 1;
}

/******************************************************************************
 * memory_alloc
 * Memory allocation
 *
 * [in] size : size of the memory to allocate (in byte)
 *
 * Return the pointer of the allocated size or null if error
 *****************************************************************************/
void * memory_alloc(unsigned long size)
{
  memory_level_t level;
  memory_block_t * new_block;

  /* As minimum block size is 16 bytes, check the size */
  if(size > BLOCK_MIN_SIZE)
  {
    /* Size must have the last two bits at 0 due to free and last bit flags */
    size = RESIZE_UP(size, LONG_SIZE_BYTE);
    /* Get the next level  to be sur to have the minimum required size */
    block_get_next_level(size, &level);
  }
  else
  {
    size = BLOCK_MIN_SIZE;
    level.fl = 0;
    level.fl_bitmap = 1;
    level.sl = 4;
    level.sl_bitmap = 16;
  }

  /* Check if exists a free block */
  new_block = block_find(&level);

  /* If not return null */
  if(new_block == NULL)
    return NULL;

  /* Extract free block from the chain list */
  block_extract(new_block);

  /* Split the new block */
  block_split(new_block, size);

  return (void *)((unsigned long)new_block + BLOCK_HEADER_SIZE_USED);
}

/******************************************************************************
 * memory_free
 * Free a memory previously allocated
 *
 * [in] ptr : pointer the memory to free
 *****************************************************************************/
void memory_free(void * ptr)
{
  memory_block_t *current_block = (memory_block_t *) ((unsigned long) ptr - BLOCK_HEADER_SIZE_USED);

  /* Check if the current block is used */
  if(BLOCK_IS_FREE(current_block))
    return;

  block_insert(block_merge_left(block_merge_right(current_block)));
}

