/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_INSTRUMENTATION_H
#define __LINUX_INSTRUMENTATION_H

#if defined(CONFIG_DEBUG_ENTRY) && defined(CONFIG_STACK_VALIDATION)


#define instrumentation_begin() ({					\
	asm volatile("%c0: nop\n\t"						\
		     ".pushsection .discard.instr_begin\n\t"		\
		     ".long %c0b - .\n\t"				\
		     ".popsection\n\t" : : "i" (__COUNTER__));		\
})


#define instrumentation_end() ({					\
	asm volatile("%c0: nop\n\t"					\
		     ".pushsection .discard.instr_end\n\t"		\
		     ".long %c0b - .\n\t"				\
		     ".popsection\n\t" : : "i" (__COUNTER__));		\
})
#else
# define instrumentation_begin()	do { } while(0)
# define instrumentation_end()		do { } while(0)
#endif

#endif 
