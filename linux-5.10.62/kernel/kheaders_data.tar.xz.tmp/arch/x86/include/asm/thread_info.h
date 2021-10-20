/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _ASM_X86_THREAD_INFO_H
#define _ASM_X86_THREAD_INFO_H

#include <linux/compiler.h>
#include <asm/page.h>
#include <asm/percpu.h>
#include <asm/types.h>


#ifdef CONFIG_X86_32
# ifdef CONFIG_VM86
#  define TOP_OF_KERNEL_STACK_PADDING 16
# else
#  define TOP_OF_KERNEL_STACK_PADDING 8
# endif
#else
# define TOP_OF_KERNEL_STACK_PADDING 0
#endif


#ifndef __ASSEMBLY__
struct task_struct;
#include <asm/cpufeature.h>
#include <linux/atomic.h>

struct thread_info {
	unsigned long		flags;		
	u32			status;		
};

#define INIT_THREAD_INFO(tsk)			\
{						\
	.flags		= 0,			\
}

#else 

#include <asm/asm-offsets.h>

#endif


#define TIF_SYSCALL_TRACE	0	
#define TIF_NOTIFY_RESUME	1	
#define TIF_SIGPENDING		2	
#define TIF_NEED_RESCHED	3	
#define TIF_SINGLESTEP		4	
#define TIF_SSBD		5	
#define TIF_SYSCALL_EMU		6	
#define TIF_SYSCALL_AUDIT	7	
#define TIF_SECCOMP		8	
#define TIF_SPEC_IB		9	
#define TIF_SPEC_FORCE_UPDATE	10	
#define TIF_USER_RETURN_NOTIFY	11	
#define TIF_UPROBE		12	
#define TIF_PATCH_PENDING	13	
#define TIF_NEED_FPU_LOAD	14	
#define TIF_NOCPUID		15	
#define TIF_NOTSC		16	
#define TIF_IA32		17	
#define TIF_SLD			18	
#define TIF_MEMDIE		20	
#define TIF_POLLING_NRFLAG	21	
#define TIF_IO_BITMAP		22	
#define TIF_FORCED_TF		24	
#define TIF_BLOCKSTEP		25	
#define TIF_LAZY_MMU_UPDATES	27	
#define TIF_SYSCALL_TRACEPOINT	28	
#define TIF_ADDR32		29	
#define TIF_X32			30	

#define _TIF_SYSCALL_TRACE	(1 << TIF_SYSCALL_TRACE)
#define _TIF_NOTIFY_RESUME	(1 << TIF_NOTIFY_RESUME)
#define _TIF_SIGPENDING		(1 << TIF_SIGPENDING)
#define _TIF_NEED_RESCHED	(1 << TIF_NEED_RESCHED)
#define _TIF_SINGLESTEP		(1 << TIF_SINGLESTEP)
#define _TIF_SSBD		(1 << TIF_SSBD)
#define _TIF_SYSCALL_EMU	(1 << TIF_SYSCALL_EMU)
#define _TIF_SYSCALL_AUDIT	(1 << TIF_SYSCALL_AUDIT)
#define _TIF_SECCOMP		(1 << TIF_SECCOMP)
#define _TIF_SPEC_IB		(1 << TIF_SPEC_IB)
#define _TIF_SPEC_FORCE_UPDATE	(1 << TIF_SPEC_FORCE_UPDATE)
#define _TIF_USER_RETURN_NOTIFY	(1 << TIF_USER_RETURN_NOTIFY)
#define _TIF_UPROBE		(1 << TIF_UPROBE)
#define _TIF_PATCH_PENDING	(1 << TIF_PATCH_PENDING)
#define _TIF_NEED_FPU_LOAD	(1 << TIF_NEED_FPU_LOAD)
#define _TIF_NOCPUID		(1 << TIF_NOCPUID)
#define _TIF_NOTSC		(1 << TIF_NOTSC)
#define _TIF_IA32		(1 << TIF_IA32)
#define _TIF_SLD		(1 << TIF_SLD)
#define _TIF_POLLING_NRFLAG	(1 << TIF_POLLING_NRFLAG)
#define _TIF_IO_BITMAP		(1 << TIF_IO_BITMAP)
#define _TIF_FORCED_TF		(1 << TIF_FORCED_TF)
#define _TIF_BLOCKSTEP		(1 << TIF_BLOCKSTEP)
#define _TIF_LAZY_MMU_UPDATES	(1 << TIF_LAZY_MMU_UPDATES)
#define _TIF_SYSCALL_TRACEPOINT	(1 << TIF_SYSCALL_TRACEPOINT)
#define _TIF_ADDR32		(1 << TIF_ADDR32)
#define _TIF_X32		(1 << TIF_X32)


#define _TIF_WORK_CTXSW_BASE					\
	(_TIF_NOCPUID | _TIF_NOTSC | _TIF_BLOCKSTEP |		\
	 _TIF_SSBD | _TIF_SPEC_FORCE_UPDATE | _TIF_SLD)


#ifdef CONFIG_SMP
# define _TIF_WORK_CTXSW	(_TIF_WORK_CTXSW_BASE | _TIF_SPEC_IB)
#else
# define _TIF_WORK_CTXSW	(_TIF_WORK_CTXSW_BASE)
#endif

#ifdef CONFIG_X86_IOPL_IOPERM
# define _TIF_WORK_CTXSW_PREV	(_TIF_WORK_CTXSW| _TIF_USER_RETURN_NOTIFY | \
				 _TIF_IO_BITMAP)
#else
# define _TIF_WORK_CTXSW_PREV	(_TIF_WORK_CTXSW| _TIF_USER_RETURN_NOTIFY)
#endif

#define _TIF_WORK_CTXSW_NEXT	(_TIF_WORK_CTXSW)

#define STACK_WARN		(THREAD_SIZE/8)


#ifndef __ASSEMBLY__


static inline int arch_within_stack_frames(const void * const stack,
					   const void * const stackend,
					   const void *obj, unsigned long len)
{
#if defined(CONFIG_FRAME_POINTER)
	const void *frame = NULL;
	const void *oldframe;

	oldframe = __builtin_frame_address(1);
	if (oldframe)
		frame = __builtin_frame_address(2);
	
	while (stack <= frame && frame < stackend) {
		
		if (obj + len <= frame)
			return obj >= oldframe + 2 * sizeof(void *) ?
				GOOD_FRAME : BAD_STACK;
		oldframe = frame;
		frame = *(const void * const *)frame;
	}
	return BAD_STACK;
#else
	return NOT_STACK;
#endif
}

#else 

#ifdef CONFIG_X86_64
# define cpu_current_top_of_stack (cpu_tss_rw + TSS_sp1)
#endif

#endif


#define TS_COMPAT		0x0002	

#ifndef __ASSEMBLY__
#ifdef CONFIG_COMPAT
#define TS_I386_REGS_POKED	0x0004	
#define TS_COMPAT_RESTART	0x0008

#define arch_set_restart_data	arch_set_restart_data

static inline void arch_set_restart_data(struct restart_block *restart)
{
	struct thread_info *ti = current_thread_info();
	if (ti->status & TS_COMPAT)
		ti->status |= TS_COMPAT_RESTART;
	else
		ti->status &= ~TS_COMPAT_RESTART;
}
#endif

#ifdef CONFIG_X86_32
#define in_ia32_syscall() true
#else
#define in_ia32_syscall() (IS_ENABLED(CONFIG_IA32_EMULATION) && \
			   current_thread_info()->status & TS_COMPAT)
#endif

extern void arch_task_cache_init(void);
extern int arch_dup_task_struct(struct task_struct *dst, struct task_struct *src);
extern void arch_release_task_struct(struct task_struct *tsk);
extern void arch_setup_new_exec(void);
#define arch_setup_new_exec arch_setup_new_exec
#endif	

#endif 
