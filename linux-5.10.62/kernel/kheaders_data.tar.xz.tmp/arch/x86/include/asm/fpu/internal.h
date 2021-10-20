/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _ASM_X86_FPU_INTERNAL_H
#define _ASM_X86_FPU_INTERNAL_H

#include <linux/compat.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/mm.h>

#include <asm/user.h>
#include <asm/fpu/api.h>
#include <asm/fpu/xstate.h>
#include <asm/fpu/xcr.h>
#include <asm/cpufeature.h>
#include <asm/trace/fpu.h>


extern void fpu__prepare_read(struct fpu *fpu);
extern void fpu__prepare_write(struct fpu *fpu);
extern void fpu__save(struct fpu *fpu);
extern int  fpu__restore_sig(void __user *buf, int ia32_frame);
extern void fpu__drop(struct fpu *fpu);
extern int  fpu__copy(struct task_struct *dst, struct task_struct *src);
extern void fpu__clear_user_states(struct fpu *fpu);
extern void fpu__clear_all(struct fpu *fpu);
extern int  fpu__exception_code(struct fpu *fpu, int trap_nr);


extern void fpu__init_cpu(void);
extern void fpu__init_system_xstate(void);
extern void fpu__init_cpu_xstate(void);
extern void fpu__init_system(struct cpuinfo_x86 *c);
extern void fpu__init_check_bugs(void);
extern void fpu__resume_cpu(void);
extern u64 fpu__get_supported_xfeatures_mask(void);


#ifdef CONFIG_X86_DEBUG_FPU
# define WARN_ON_FPU(x) WARN_ON_ONCE(x)
#else
# define WARN_ON_FPU(x) ({ (void)(x); 0; })
#endif


static __always_inline __pure bool use_xsaveopt(void)
{
	return static_cpu_has(X86_FEATURE_XSAVEOPT);
}

static __always_inline __pure bool use_xsave(void)
{
	return static_cpu_has(X86_FEATURE_XSAVE);
}

static __always_inline __pure bool use_fxsr(void)
{
	return static_cpu_has(X86_FEATURE_FXSR);
}



extern union fpregs_state init_fpstate;

extern void fpstate_init(union fpregs_state *state);
#ifdef CONFIG_MATH_EMULATION
extern void fpstate_init_soft(struct swregs_state *soft);
#else
static inline void fpstate_init_soft(struct swregs_state *soft) {}
#endif

static inline void fpstate_init_xstate(struct xregs_state *xsave)
{
	
	xsave->header.xcomp_bv = XCOMP_BV_COMPACTED_FORMAT | xfeatures_mask_all;
}

static inline void fpstate_init_fxstate(struct fxregs_state *fx)
{
	fx->cwd = 0x37f;
	fx->mxcsr = MXCSR_DEFAULT;
}
extern void fpstate_sanitize_xstate(struct fpu *fpu);


#define user_insn(insn, output, input...)				\
({									\
	int err;							\
									\
	might_fault();							\
									\
	asm volatile(ASM_STAC "\n"					\
		     "1: " #insn "\n"					\
		     "2: " ASM_CLAC "\n"				\
		     ".section .fixup,\"ax\"\n"				\
		     "3:  negl %%eax\n"					\
		     "    jmp  2b\n"					\
		     ".previous\n"					\
		     _ASM_EXTABLE_FAULT(1b, 3b)				\
		     : [err] "=a" (err), output				\
		     : "0"(0), input);					\
	err;								\
})

#define kernel_insn_err(insn, output, input...)				\
({									\
	int err;							\
	asm volatile("1:" #insn "\n\t"					\
		     "2:\n"						\
		     ".section .fixup,\"ax\"\n"				\
		     "3:  movl $-1,%[err]\n"				\
		     "    jmp  2b\n"					\
		     ".previous\n"					\
		     _ASM_EXTABLE(1b, 3b)				\
		     : [err] "=r" (err), output				\
		     : "0"(0), input);					\
	err;								\
})

#define kernel_insn(insn, output, input...)				\
	asm volatile("1:" #insn "\n\t"					\
		     "2:\n"						\
		     _ASM_EXTABLE_HANDLE(1b, 2b, ex_handler_fprestore)	\
		     : output : input)

static inline int copy_fregs_to_user(struct fregs_state __user *fx)
{
	return user_insn(fnsave %[fx]; fwait,  [fx] "=m" (*fx), "m" (*fx));
}

static inline int copy_fxregs_to_user(struct fxregs_state __user *fx)
{
	if (IS_ENABLED(CONFIG_X86_32))
		return user_insn(fxsave %[fx], [fx] "=m" (*fx), "m" (*fx));
	else
		return user_insn(fxsaveq %[fx], [fx] "=m" (*fx), "m" (*fx));

}

static inline void copy_kernel_to_fxregs(struct fxregs_state *fx)
{
	if (IS_ENABLED(CONFIG_X86_32))
		kernel_insn(fxrstor %[fx], "=m" (*fx), [fx] "m" (*fx));
	else
		kernel_insn(fxrstorq %[fx], "=m" (*fx), [fx] "m" (*fx));
}

static inline int copy_kernel_to_fxregs_err(struct fxregs_state *fx)
{
	if (IS_ENABLED(CONFIG_X86_32))
		return kernel_insn_err(fxrstor %[fx], "=m" (*fx), [fx] "m" (*fx));
	else
		return kernel_insn_err(fxrstorq %[fx], "=m" (*fx), [fx] "m" (*fx));
}

static inline int copy_user_to_fxregs(struct fxregs_state __user *fx)
{
	if (IS_ENABLED(CONFIG_X86_32))
		return user_insn(fxrstor %[fx], "=m" (*fx), [fx] "m" (*fx));
	else
		return user_insn(fxrstorq %[fx], "=m" (*fx), [fx] "m" (*fx));
}

static inline void copy_kernel_to_fregs(struct fregs_state *fx)
{
	kernel_insn(frstor %[fx], "=m" (*fx), [fx] "m" (*fx));
}

static inline int copy_kernel_to_fregs_err(struct fregs_state *fx)
{
	return kernel_insn_err(frstor %[fx], "=m" (*fx), [fx] "m" (*fx));
}

static inline int copy_user_to_fregs(struct fregs_state __user *fx)
{
	return user_insn(frstor %[fx], "=m" (*fx), [fx] "m" (*fx));
}

static inline void copy_fxregs_to_kernel(struct fpu *fpu)
{
	if (IS_ENABLED(CONFIG_X86_32))
		asm volatile( "fxsave %[fx]" : [fx] "=m" (fpu->state.fxsave));
	else
		asm volatile("fxsaveq %[fx]" : [fx] "=m" (fpu->state.fxsave));
}

static inline void fxsave(struct fxregs_state *fx)
{
	if (IS_ENABLED(CONFIG_X86_32))
		asm volatile( "fxsave %[fx]" : [fx] "=m" (*fx));
	else
		asm volatile("fxsaveq %[fx]" : [fx] "=m" (*fx));
}


#define XSAVE		".byte " REX_PREFIX "0x0f,0xae,0x27"
#define XSAVEOPT	".byte " REX_PREFIX "0x0f,0xae,0x37"
#define XSAVES		".byte " REX_PREFIX "0x0f,0xc7,0x2f"
#define XRSTOR		".byte " REX_PREFIX "0x0f,0xae,0x2f"
#define XRSTORS		".byte " REX_PREFIX "0x0f,0xc7,0x1f"


#define XSTATE_OP(op, st, lmask, hmask, err)				\
	asm volatile("1:" op "\n\t"					\
		     "xor %[err], %[err]\n"				\
		     "2:\n\t"						\
		     ".pushsection .fixup,\"ax\"\n\t"			\
		     "3: negl %%eax\n\t"				\
		     "jmp 2b\n\t"					\
		     ".popsection\n\t"					\
		     _ASM_EXTABLE_FAULT(1b, 3b)				\
		     : [err] "=a" (err)					\
		     : "D" (st), "m" (*st), "a" (lmask), "d" (hmask)	\
		     : "memory")


#define XSTATE_XSAVE(st, lmask, hmask, err)				\
	asm volatile(ALTERNATIVE_2(XSAVE,				\
				   XSAVEOPT, X86_FEATURE_XSAVEOPT,	\
				   XSAVES,   X86_FEATURE_XSAVES)	\
		     "\n"						\
		     "xor %[err], %[err]\n"				\
		     "3:\n"						\
		     ".pushsection .fixup,\"ax\"\n"			\
		     "4: movl $-2, %[err]\n"				\
		     "jmp 3b\n"						\
		     ".popsection\n"					\
		     _ASM_EXTABLE(661b, 4b)				\
		     : [err] "=r" (err)					\
		     : "D" (st), "m" (*st), "a" (lmask), "d" (hmask)	\
		     : "memory")


#define XSTATE_XRESTORE(st, lmask, hmask)				\
	asm volatile(ALTERNATIVE(XRSTOR,				\
				 XRSTORS, X86_FEATURE_XSAVES)		\
		     "\n"						\
		     "3:\n"						\
		     _ASM_EXTABLE_HANDLE(661b, 3b, ex_handler_fprestore)\
		     :							\
		     : "D" (st), "m" (*st), "a" (lmask), "d" (hmask)	\
		     : "memory")


static inline void copy_kernel_to_xregs_booting(struct xregs_state *xstate)
{
	u64 mask = -1;
	u32 lmask = mask;
	u32 hmask = mask >> 32;
	int err;

	WARN_ON(system_state != SYSTEM_BOOTING);

	if (boot_cpu_has(X86_FEATURE_XSAVES))
		XSTATE_OP(XRSTORS, xstate, lmask, hmask, err);
	else
		XSTATE_OP(XRSTOR, xstate, lmask, hmask, err);

	
	WARN_ON_FPU(err);
}


static inline void copy_xregs_to_kernel(struct xregs_state *xstate)
{
	u64 mask = xfeatures_mask_all;
	u32 lmask = mask;
	u32 hmask = mask >> 32;
	int err;

	WARN_ON_FPU(!alternatives_patched);

	XSTATE_XSAVE(xstate, lmask, hmask, err);

	
	WARN_ON_FPU(err);
}


static inline void copy_kernel_to_xregs(struct xregs_state *xstate, u64 mask)
{
	u32 lmask = mask;
	u32 hmask = mask >> 32;

	XSTATE_XRESTORE(xstate, lmask, hmask);
}


static inline int copy_xregs_to_user(struct xregs_state __user *buf)
{
	u64 mask = xfeatures_mask_user();
	u32 lmask = mask;
	u32 hmask = mask >> 32;
	int err;

	
	err = __clear_user(&buf->header, sizeof(buf->header));
	if (unlikely(err))
		return -EFAULT;

	stac();
	XSTATE_OP(XSAVE, buf, lmask, hmask, err);
	clac();

	return err;
}


static inline int copy_user_to_xregs(struct xregs_state __user *buf, u64 mask)
{
	struct xregs_state *xstate = ((__force struct xregs_state *)buf);
	u32 lmask = mask;
	u32 hmask = mask >> 32;
	int err;

	stac();
	XSTATE_OP(XRSTOR, xstate, lmask, hmask, err);
	clac();

	return err;
}


static inline int copy_kernel_to_xregs_err(struct xregs_state *xstate, u64 mask)
{
	u32 lmask = mask;
	u32 hmask = mask >> 32;
	int err;

	if (static_cpu_has(X86_FEATURE_XSAVES))
		XSTATE_OP(XRSTORS, xstate, lmask, hmask, err);
	else
		XSTATE_OP(XRSTOR, xstate, lmask, hmask, err);

	return err;
}

extern int copy_fpregs_to_fpstate(struct fpu *fpu);

static inline void __copy_kernel_to_fpregs(union fpregs_state *fpstate, u64 mask)
{
	if (use_xsave()) {
		copy_kernel_to_xregs(&fpstate->xsave, mask);
	} else {
		if (use_fxsr())
			copy_kernel_to_fxregs(&fpstate->fxsave);
		else
			copy_kernel_to_fregs(&fpstate->fsave);
	}
}

static inline void copy_kernel_to_fpregs(union fpregs_state *fpstate)
{
	
	if (unlikely(static_cpu_has_bug(X86_BUG_FXSAVE_LEAK))) {
		asm volatile(
			"fnclex\n\t"
			"emms\n\t"
			"fildl %P[addr]"	
			: : [addr] "m" (fpstate));
	}

	__copy_kernel_to_fpregs(fpstate, -1);
}

extern int copy_fpstate_to_sigframe(void __user *buf, void __user *fp, int size);



DECLARE_PER_CPU(struct fpu *, fpu_fpregs_owner_ctx);


static inline void __cpu_invalidate_fpregs_state(void)
{
	__this_cpu_write(fpu_fpregs_owner_ctx, NULL);
}

static inline void __fpu_invalidate_fpregs_state(struct fpu *fpu)
{
	fpu->last_cpu = -1;
}

static inline int fpregs_state_valid(struct fpu *fpu, unsigned int cpu)
{
	return fpu == this_cpu_read(fpu_fpregs_owner_ctx) && cpu == fpu->last_cpu;
}


static inline void fpregs_deactivate(struct fpu *fpu)
{
	this_cpu_write(fpu_fpregs_owner_ctx, NULL);
	trace_x86_fpu_regs_deactivated(fpu);
}

static inline void fpregs_activate(struct fpu *fpu)
{
	this_cpu_write(fpu_fpregs_owner_ctx, fpu);
	trace_x86_fpu_regs_activated(fpu);
}


static inline void __fpregs_load_activate(void)
{
	struct fpu *fpu = &current->thread.fpu;
	int cpu = smp_processor_id();

	if (WARN_ON_ONCE(current->flags & PF_KTHREAD))
		return;

	if (!fpregs_state_valid(fpu, cpu)) {
		copy_kernel_to_fpregs(&fpu->state);
		fpregs_activate(fpu);
		fpu->last_cpu = cpu;
	}
	clear_thread_flag(TIF_NEED_FPU_LOAD);
}


static inline void switch_fpu_prepare(struct fpu *old_fpu, int cpu)
{
	if (static_cpu_has(X86_FEATURE_FPU) && !(current->flags & PF_KTHREAD)) {
		if (!copy_fpregs_to_fpstate(old_fpu))
			old_fpu->last_cpu = -1;
		else
			old_fpu->last_cpu = cpu;

		
		trace_x86_fpu_regs_deactivated(old_fpu);
	}
}




static inline void switch_fpu_finish(struct fpu *new_fpu)
{
	u32 pkru_val = init_pkru_value;
	struct pkru_state *pk;

	if (!static_cpu_has(X86_FEATURE_FPU))
		return;

	set_thread_flag(TIF_NEED_FPU_LOAD);

	if (!cpu_feature_enabled(X86_FEATURE_OSPKE))
		return;

	
	if (!(current->flags & PF_KTHREAD)) {
		
		pk = get_xsave_addr(&new_fpu->state.xsave, XFEATURE_PKRU);
		pkru_val = pk ? pk->pkru : 0;
	}
	__write_pkru(pkru_val);
}

#endif 
