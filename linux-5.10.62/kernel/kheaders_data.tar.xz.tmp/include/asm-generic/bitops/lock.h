/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_GENERIC_BITOPS_LOCK_H_
#define _ASM_GENERIC_BITOPS_LOCK_H_

#include <linux/atomic.h>
#include <linux/compiler.h>
#include <asm/barrier.h>


static inline int test_and_set_bit_lock(unsigned int nr,
					volatile unsigned long *p)
{
	long old;
	unsigned long mask = BIT_MASK(nr);

	p += BIT_WORD(nr);
	if (READ_ONCE(*p) & mask)
		return 1;

	old = atomic_long_fetch_or_acquire(mask, (atomic_long_t *)p);
	return !!(old & mask);
}



static inline void clear_bit_unlock(unsigned int nr, volatile unsigned long *p)
{
	p += BIT_WORD(nr);
	atomic_long_fetch_andnot_release(BIT_MASK(nr), (atomic_long_t *)p);
}


static inline void __clear_bit_unlock(unsigned int nr,
				      volatile unsigned long *p)
{
	unsigned long old;

	p += BIT_WORD(nr);
	old = READ_ONCE(*p);
	old &= ~BIT_MASK(nr);
	atomic_long_set_release((atomic_long_t *)p, old);
}


#ifndef clear_bit_unlock_is_negative_byte
static inline bool clear_bit_unlock_is_negative_byte(unsigned int nr,
						     volatile unsigned long *p)
{
	long old;
	unsigned long mask = BIT_MASK(nr);

	p += BIT_WORD(nr);
	old = atomic_long_fetch_andnot_release(mask, (atomic_long_t *)p);
	return !!(old & BIT(7));
}
#define clear_bit_unlock_is_negative_byte clear_bit_unlock_is_negative_byte
#endif

#endif 
