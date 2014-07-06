/*  This file is part of FMA32
    Fast Memory Allocator for 32 bits embedded system.
    (Romain CARITEY - 2014)

    FMA32 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Foobar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MEMORY_H
#define MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL                                    (void*)(0)
#endif /* NULL */

#define offsetof(type,member)                   ((unsigned long) &(((type*)0)->member))

#define LONG_SIZE_BYTE													sizeof(unsigned long)
#define LONG_SIZE_BIT														(LONG_SIZE_BYTE * 8)
#define ALIGN_MASK                              (LONG_SIZE_BYTE - 1)

#define RESIZE_UP(val, base)                    (((unsigned long)(val) + ((base)-1)) & ~((base)-1))
#define RESIZE_DOWN(val, base)                  ((unsigned long)(val) & ~((base)-1))

#define BLOCK_FREE_BIT		                      0x1UL
#define BLOCK_LAST_BIT                         	0x2UL
#define BLOCK_BIT_MASK                          (BLOCK_FREE_BIT | BLOCK_LAST_BIT)

#define BLOCK_IS_FREE(block)                    ((block->size & BLOCK_FREE_BIT) ? 1 : 0)
#define BLOCK_IS_USED(block)                    ((block->size & BLOCK_FREE_BIT) ? 0 : 1)
#define BLOCK_IS_LAST(block)                    ((block->size & BLOCK_LAST_BIT) ? 1 : 0)

#define BLOCK_MARK_AS_LAST(block)              	((block)->size |= BLOCK_LAST_BIT)
#define BLOCK_MARK_AS_NOT_LAST(block)           ((block)->size &= ~BLOCK_LAST_BIT)
#define BLOCK_MARK_AS_FREE(block)               ((block)->size |= BLOCK_FREE_BIT)
#define BLOCK_MARK_AS_USED(block)               ((block)->size &= (~BLOCK_FREE_BIT))

#define BLOCK_GET_MASKED_SIZE(block)   					((block)->size & (~BLOCK_BIT_MASK))
#define BLOCK_GET_FLAG_BIT(block)               ((block)->size & BLOCK_BIT_MASK)
#define BLOCK_MASK_FREE(level)                  (~((level)-1))

#define BLOCK_MIN_SIZE				                  sizeof(memory_block_t)
#define BLOCK_HEADER_SIZE_FREE                  BLOCK_MIN_SIZE
#define BLOCK_HEADER_SIZE_USED				          (unsigned long)offsetof(memory_block_t, prev)

typedef struct memory_block_s {
  unsigned long size;
  struct memory_block_s *phys_prev;
  struct memory_block_s *prev;
  struct memory_block_s *next;
} memory_block_t;

typedef struct memory_management_area_s {
  unsigned long * first_level;
  unsigned long * second_level;
  memory_block_t *(*fbla)[LONG_SIZE_BIT];
} memory_management_area_t;

typedef struct {
  unsigned long fl;
  unsigned long fl_bitmap;
  unsigned long sl;
  unsigned long sl_bitmap;
} memory_level_t;

unsigned long memory_init(void * mem_ptr, unsigned long length);
void * memory_alloc(unsigned long size);
void memory_free(void * ptr);

#ifdef TEST_MODE
#define STATIC
#else
#define STATIC static
#endif /* TEST_MODE */

#ifdef __cplusplus
}
#endif

#endif /* MEMORY_H */
