/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_GENERIC_BITOPS_FIND_H_
#define _ASM_GENERIC_BITOPS_FIND_H_

#ifndef find_next_bit

extern unsigned long find_next_bit(const unsigned long *addr, unsigned long
		size, unsigned long offset);
#endif

#ifndef find_next_and_bit

extern unsigned long find_next_and_bit(const unsigned long *addr1,
		const unsigned long *addr2, unsigned long size,
		unsigned long offset);
#endif

#ifndef find_next_zero_bit

extern unsigned long find_next_zero_bit(const unsigned long *addr, unsigned
		long size, unsigned long offset);
#endif

#ifdef CONFIG_GENERIC_FIND_FIRST_BIT


extern unsigned long find_first_bit(const unsigned long *addr,
				    unsigned long size);


extern unsigned long find_first_zero_bit(const unsigned long *addr,
					 unsigned long size);
#else 

#ifndef find_first_bit
#define find_first_bit(addr, size) find_next_bit((addr), (size), 0)
#endif
#ifndef find_first_zero_bit
#define find_first_zero_bit(addr, size) find_next_zero_bit((addr), (size), 0)
#endif

#endif 


extern unsigned long find_next_clump8(unsigned long *clump,
				      const unsigned long *addr,
				      unsigned long size, unsigned long offset);

#define find_first_clump8(clump, bits, size) \
	find_next_clump8((clump), (bits), (size), 0)

#endif 
