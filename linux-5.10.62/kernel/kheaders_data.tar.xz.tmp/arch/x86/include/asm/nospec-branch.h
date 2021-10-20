/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _ASM_X86_NOSPEC_BRANCH_H_
#define _ASM_X86_NOSPEC_BRANCH_H_

#include <linux/static_key.h>
#include <linux/objtool.h>

#include <asm/alternative.h>
#include <asm/alternative-asm.h>
#include <asm/cpufeatures.h>
#include <asm/msr-index.h>
#include <asm/unwind_hints.h>



#define RSB_CLEAR_LOOPS		32	


#define __FILL_RETURN_BUFFER(reg, nr, sp)	\
	mov	$(nr/2), reg;			\
771:						\
	ANNOTATE_INTRA_FUNCTION_CALL;		\
	call	772f;				\
773:				\
	UNWIND_HINT_EMPTY;			\
	pause;					\
	lfence;					\
	jmp	773b;				\
772:						\
	ANNOTATE_INTRA_FUNCTION_CALL;		\
	call	774f;				\
775:				\
	UNWIND_HINT_EMPTY;			\
	pause;					\
	lfence;					\
	jmp	775b;				\
774:						\
	add	$(BITS_PER_LONG/8) * 2, sp;	\
	dec	reg;				\
	jnz	771b;

#ifdef __ASSEMBLY__


.macro ANNOTATE_RETPOLINE_SAFE
	.Lannotate_\@:
	.pushsection .discard.retpoline_safe
	_ASM_PTR .Lannotate_\@
	.popsection
.endm


.macro JMP_NOSPEC reg:req
#ifdef CONFIG_RETPOLINE
	ALTERNATIVE_2 __stringify(ANNOTATE_RETPOLINE_SAFE; jmp *%\reg), \
		      __stringify(jmp __x86_retpoline_\reg), X86_FEATURE_RETPOLINE, \
		      __stringify(lfence; ANNOTATE_RETPOLINE_SAFE; jmp *%\reg), X86_FEATURE_RETPOLINE_AMD
#else
	jmp	*%\reg
#endif
.endm

.macro CALL_NOSPEC reg:req
#ifdef CONFIG_RETPOLINE
	ALTERNATIVE_2 __stringify(ANNOTATE_RETPOLINE_SAFE; call *%\reg), \
		      __stringify(call __x86_retpoline_\reg), X86_FEATURE_RETPOLINE, \
		      __stringify(lfence; ANNOTATE_RETPOLINE_SAFE; call *%\reg), X86_FEATURE_RETPOLINE_AMD
#else
	call	*%\reg
#endif
.endm

 
.macro FILL_RETURN_BUFFER reg:req nr:req ftr:req
#ifdef CONFIG_RETPOLINE
	ALTERNATIVE "jmp .Lskip_rsb_\@", "", \ftr
	__FILL_RETURN_BUFFER(\reg,\nr,%_ASM_SP)
.Lskip_rsb_\@:
#endif
.endm

#else 

#define ANNOTATE_RETPOLINE_SAFE					\
	"999:\n\t"						\
	".pushsection .discard.retpoline_safe\n\t"		\
	_ASM_PTR " 999b\n\t"					\
	".popsection\n\t"

#ifdef CONFIG_RETPOLINE
#ifdef CONFIG_X86_64


# define CALL_NOSPEC						\
	ALTERNATIVE_2(						\
	ANNOTATE_RETPOLINE_SAFE					\
	"call *%[thunk_target]\n",				\
	"call __x86_retpoline_%V[thunk_target]\n",		\
	X86_FEATURE_RETPOLINE,					\
	"lfence;\n"						\
	ANNOTATE_RETPOLINE_SAFE					\
	"call *%[thunk_target]\n",				\
	X86_FEATURE_RETPOLINE_AMD)

# define THUNK_TARGET(addr) [thunk_target] "r" (addr)

#else 

# define CALL_NOSPEC						\
	ALTERNATIVE_2(						\
	ANNOTATE_RETPOLINE_SAFE					\
	"call *%[thunk_target]\n",				\
	"       jmp    904f;\n"					\
	"       .align 16\n"					\
	"901:	call   903f;\n"					\
	"902:	pause;\n"					\
	"    	lfence;\n"					\
	"       jmp    902b;\n"					\
	"       .align 16\n"					\
	"903:	lea    4(%%esp), %%esp;\n"			\
	"       pushl  %[thunk_target];\n"			\
	"       ret;\n"						\
	"       .align 16\n"					\
	"904:	call   901b;\n",				\
	X86_FEATURE_RETPOLINE,					\
	"lfence;\n"						\
	ANNOTATE_RETPOLINE_SAFE					\
	"call *%[thunk_target]\n",				\
	X86_FEATURE_RETPOLINE_AMD)

# define THUNK_TARGET(addr) [thunk_target] "rm" (addr)
#endif
#else 
# define CALL_NOSPEC "call *%[thunk_target]\n"
# define THUNK_TARGET(addr) [thunk_target] "rm" (addr)
#endif


enum spectre_v2_mitigation {
	SPECTRE_V2_NONE,
	SPECTRE_V2_RETPOLINE_GENERIC,
	SPECTRE_V2_RETPOLINE_AMD,
	SPECTRE_V2_IBRS_ENHANCED,
};


enum spectre_v2_user_mitigation {
	SPECTRE_V2_USER_NONE,
	SPECTRE_V2_USER_STRICT,
	SPECTRE_V2_USER_STRICT_PREFERRED,
	SPECTRE_V2_USER_PRCTL,
	SPECTRE_V2_USER_SECCOMP,
};


enum ssb_mitigation {
	SPEC_STORE_BYPASS_NONE,
	SPEC_STORE_BYPASS_DISABLE,
	SPEC_STORE_BYPASS_PRCTL,
	SPEC_STORE_BYPASS_SECCOMP,
};

extern char __indirect_thunk_start[];
extern char __indirect_thunk_end[];

static __always_inline
void alternative_msr_write(unsigned int msr, u64 val, unsigned int feature)
{
	asm volatile(ALTERNATIVE("", "wrmsr", %c[feature])
		: : "c" (msr),
		    "a" ((u32)val),
		    "d" ((u32)(val >> 32)),
		    [feature] "i" (feature)
		: "memory");
}

static inline void indirect_branch_prediction_barrier(void)
{
	u64 val = PRED_CMD_IBPB;

	alternative_msr_write(MSR_IA32_PRED_CMD, val, X86_FEATURE_USE_IBPB);
}


extern u64 x86_spec_ctrl_base;


#define firmware_restrict_branch_speculation_start()			\
do {									\
	u64 val = x86_spec_ctrl_base | SPEC_CTRL_IBRS;			\
									\
	preempt_disable();						\
	alternative_msr_write(MSR_IA32_SPEC_CTRL, val,			\
			      X86_FEATURE_USE_IBRS_FW);			\
} while (0)

#define firmware_restrict_branch_speculation_end()			\
do {									\
	u64 val = x86_spec_ctrl_base;					\
									\
	alternative_msr_write(MSR_IA32_SPEC_CTRL, val,			\
			      X86_FEATURE_USE_IBRS_FW);			\
	preempt_enable();						\
} while (0)

DECLARE_STATIC_KEY_FALSE(switch_to_cond_stibp);
DECLARE_STATIC_KEY_FALSE(switch_mm_cond_ibpb);
DECLARE_STATIC_KEY_FALSE(switch_mm_always_ibpb);

DECLARE_STATIC_KEY_FALSE(mds_user_clear);
DECLARE_STATIC_KEY_FALSE(mds_idle_clear);

#include <asm/segment.h>


static __always_inline void mds_clear_cpu_buffers(void)
{
	static const u16 ds = __KERNEL_DS;

	
	asm volatile("verw %[ds]" : : [ds] "m" (ds) : "cc");
}


static __always_inline void mds_user_clear_cpu_buffers(void)
{
	if (static_branch_likely(&mds_user_clear))
		mds_clear_cpu_buffers();
}


static inline void mds_idle_clear_cpu_buffers(void)
{
	if (static_branch_likely(&mds_idle_clear))
		mds_clear_cpu_buffers();
}

#endif 


#ifdef CONFIG_RETPOLINE
# ifdef CONFIG_X86_64
#  define RETPOLINE_RCX_BPF_JIT_SIZE	17
#  define RETPOLINE_RCX_BPF_JIT()				\
do {								\
	EMIT1_off32(0xE8, 7);	 		\
						\
	EMIT2(0xF3, 0x90);       			\
	EMIT3(0x0F, 0xAE, 0xE8); 			\
	EMIT2(0xEB, 0xF9);       		\
							\
	EMIT4(0x48, 0x89, 0x0C, 0x24); 	\
	EMIT1(0xC3);             			\
} while (0)
# else 
#  define RETPOLINE_EDX_BPF_JIT()				\
do {								\
	EMIT1_off32(0xE8, 7);	 		\
						\
	EMIT2(0xF3, 0x90);       			\
	EMIT3(0x0F, 0xAE, 0xE8); 			\
	EMIT2(0xEB, 0xF9);       		\
							\
	EMIT3(0x89, 0x14, 0x24); 		\
	EMIT1(0xC3);             			\
} while (0)
# endif
#else 
# ifdef CONFIG_X86_64
#  define RETPOLINE_RCX_BPF_JIT_SIZE	2
#  define RETPOLINE_RCX_BPF_JIT()				\
	EMIT2(0xFF, 0xE1);       
# else 
#  define RETPOLINE_EDX_BPF_JIT()				\
	EMIT2(0xFF, 0xE2)        
# endif
#endif

#endif 
