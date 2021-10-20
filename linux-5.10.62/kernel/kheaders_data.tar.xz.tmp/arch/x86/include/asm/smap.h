/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _ASM_X86_SMAP_H
#define _ASM_X86_SMAP_H

#include <asm/nops.h>
#include <asm/cpufeatures.h>


#define __ASM_CLAC	".byte 0x0f,0x01,0xca"
#define __ASM_STAC	".byte 0x0f,0x01,0xcb"

#ifdef __ASSEMBLY__

#include <asm/alternative-asm.h>

#ifdef CONFIG_X86_SMAP

#define ASM_CLAC \
	ALTERNATIVE "", __ASM_CLAC, X86_FEATURE_SMAP

#define ASM_STAC \
	ALTERNATIVE "", __ASM_STAC, X86_FEATURE_SMAP

#else 

#define ASM_CLAC
#define ASM_STAC

#endif 

#else 

#include <asm/alternative.h>

#ifdef CONFIG_X86_SMAP

static __always_inline void clac(void)
{
	
	alternative("", __ASM_CLAC, X86_FEATURE_SMAP);
}

static __always_inline void stac(void)
{
	
	alternative("", __ASM_STAC, X86_FEATURE_SMAP);
}

static __always_inline unsigned long smap_save(void)
{
	unsigned long flags;

	asm volatile ("# smap_save\n\t"
		      ALTERNATIVE("jmp 1f", "", X86_FEATURE_SMAP)
		      "pushf; pop %0; " __ASM_CLAC "\n\t"
		      "1:"
		      : "=rm" (flags) : : "memory", "cc");

	return flags;
}

static __always_inline void smap_restore(unsigned long flags)
{
	asm volatile ("# smap_restore\n\t"
		      ALTERNATIVE("jmp 1f", "", X86_FEATURE_SMAP)
		      "push %0; popf\n\t"
		      "1:"
		      : : "g" (flags) : "memory", "cc");
}


#define ASM_CLAC \
	ALTERNATIVE("", __ASM_CLAC, X86_FEATURE_SMAP)
#define ASM_STAC \
	ALTERNATIVE("", __ASM_STAC, X86_FEATURE_SMAP)

#else 

static inline void clac(void) { }
static inline void stac(void) { }

static inline unsigned long smap_save(void) { return 0; }
static inline void smap_restore(unsigned long flags) { }

#define ASM_CLAC
#define ASM_STAC

#endif 

#endif 

#endif 
