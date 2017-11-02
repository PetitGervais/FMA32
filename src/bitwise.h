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
    along with FMA.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BITWISE_H
#define BITWISE_H

#define BIT(pos)			(1 << (pos))    /* Define a bit */
#define CHAR_BIT      8               /* 8 bits per byte */

inline unsigned long bit_set(register unsigned long value, register unsigned long pos)
{
  return (value |= BIT(pos));
}

inline unsigned long bit_clear(register unsigned long value, register unsigned long pos)
{
  return (value &= ~BIT(pos));
}

inline unsigned long bit_toggle(register unsigned long value, register unsigned long pos)
{
  return (value ^= BIT(pos));
}

inline unsigned long bit_check(register unsigned long value, register unsigned long pos)
{
  return (value & BIT(pos) ? 1 : 0);
}

inline unsigned long bit_mask(register unsigned long pos)
{
  return (BIT(pos) - 1);
}

inline unsigned long is_power_of_two(unsigned long value)
{
  return ((value != 0) && !(value & (value - 1)));
}


inline unsigned long bit_lowest_pos(register unsigned long value)
{
  unsigned long pos = 0;
  while(!(value & 1))
  {
    value >>= 1;
    ++pos;
  }
  return pos;
}

inline unsigned long bit_highest_pos(register unsigned long value)
{
  register unsigned long res;
  register unsigned long shift;

  res = (value > 0xFFFF) << 4;
  value >>= res;
  shift = (value > 0xFF) << 3;
  value >>= shift;
  res |= shift;
  shift = (value > 0xF) << 2;
  value >>= shift;
  res |= shift;
  shift = (value > 0x3) << 1;
  value >>= shift;
  res |= shift;
  res |= (value >> 1);
  return res;
}

inline unsigned long bit_highest(register unsigned long value)
{
  return (1 << bit_highest_pos(value));
}

inline unsigned long bitfield_mask(register unsigned long start, register unsigned long length)
{
  return (bit_mask(length) << (start));
}

inline unsigned long bitfield_extract(register unsigned long value, register unsigned long start, register unsigned long length)
{
  return ((value >> start) & bit_mask(length));
}

inline unsigned long bit_next_power_of_two(register unsigned long value)
{
  if(!(value & (value - 1)))
    return (value);
  while (value & (value - 1))
    value = value & (value - 1);
  value = value << 1;
  return value;
}

inline unsigned long bit_rotate_left(unsigned long value, unsigned long shift)
{
  return (value << shift) | (value >> (sizeof(value) * CHAR_BIT - shift));
}

inline unsigned long bit_rotate_right(unsigned long value, unsigned long shift)
{
  return (value >> shift) | (value << (sizeof(value) * CHAR_BIT - shift));
}

#endif /* BITWISE_H */
