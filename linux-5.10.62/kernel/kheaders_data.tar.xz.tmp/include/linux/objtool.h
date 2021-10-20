/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_OBJTOOL_H
#define _LINUX_OBJTOOL_H

#ifndef __ASSEMBLY__

#include <linux/types.h>


struct unwind_hint {
	u32		ip;
	s16		sp_offset;
	u8		sp_reg;
	u8		type;
	u8		end;
};
#endif


#define UNWIND_HINT_TYPE_CALL		0
#define UNWIND_HINT_TYPE_REGS		1
#define UNWIND_HINT_TYPE_REGS_PARTIAL	2
#define UNWIND_HINT_TYPE_RET_OFFSET	3

#ifdef CONFIG_STACK_VALIDATION

#ifndef __ASSEMBLY__

#define UNWIND_HINT(sp_reg, sp_offset, type, end)		\
	"987: \n\t"						\
	".pushsection .discard.unwind_hints\n\t"		\
					\
	".long 987b - .\n\t"					\
	".short " __stringify(sp_offset) "\n\t"			\
	".byte " __stringify(sp_reg) "\n\t"			\
	".byte " __stringify(type) "\n\t"			\
	".byte " __stringify(end) "\n\t"			\
	".balign 4 \n\t"					\
	".popsection\n\t"


#define STACK_FRAME_NON_STANDARD(func) \
	static void __used __section(".discard.func_stack_frame_non_standard") \
		*__func_stack_frame_non_standard_##func = func

#else 


#define ANNOTATE_INTRA_FUNCTION_CALL				\
	999:							\
	.pushsection .discard.intra_function_calls;		\
	.long 999b;						\
	.popsection;


.macro UNWIND_HINT sp_reg:req sp_offset=0 type:req end=0
.Lunwind_hint_ip_\@:
	.pushsection .discard.unwind_hints
		
		.long .Lunwind_hint_ip_\@ - .
		.short \sp_offset
		.byte \sp_reg
		.byte \type
		.byte \end
		.balign 4
	.popsection
.endm

#endif 

#else 

#ifndef __ASSEMBLY__

#define UNWIND_HINT(sp_reg, sp_offset, type, end)	\
	"\n\t"
#define STACK_FRAME_NON_STANDARD(func)
#else
#define ANNOTATE_INTRA_FUNCTION_CALL
.macro UNWIND_HINT sp_reg:req sp_offset=0 type:req end=0
.endm
#endif

#endif 

#endif 
