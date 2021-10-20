/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_ENTRYCOMMON_H
#define __LINUX_ENTRYCOMMON_H

#include <linux/tracehook.h>
#include <linux/syscalls.h>
#include <linux/seccomp.h>
#include <linux/sched.h>

#include <asm/entry-common.h>


#ifndef _TIF_SYSCALL_EMU
# define _TIF_SYSCALL_EMU		(0)
#endif

#ifndef _TIF_SYSCALL_TRACEPOINT
# define _TIF_SYSCALL_TRACEPOINT	(0)
#endif

#ifndef _TIF_SECCOMP
# define _TIF_SECCOMP			(0)
#endif

#ifndef _TIF_SYSCALL_AUDIT
# define _TIF_SYSCALL_AUDIT		(0)
#endif

#ifndef _TIF_PATCH_PENDING
# define _TIF_PATCH_PENDING		(0)
#endif

#ifndef _TIF_UPROBE
# define _TIF_UPROBE			(0)
#endif


#ifndef ARCH_SYSCALL_ENTER_WORK
# define ARCH_SYSCALL_ENTER_WORK	(0)
#endif

#define SYSCALL_ENTER_WORK						\
	(_TIF_SYSCALL_TRACE | _TIF_SYSCALL_AUDIT | _TIF_SECCOMP |	\
	 _TIF_SYSCALL_TRACEPOINT | _TIF_SYSCALL_EMU |			\
	 ARCH_SYSCALL_ENTER_WORK)


#ifndef ARCH_SYSCALL_EXIT_WORK
# define ARCH_SYSCALL_EXIT_WORK		(0)
#endif

#define SYSCALL_EXIT_WORK						\
	(_TIF_SYSCALL_TRACE | _TIF_SYSCALL_AUDIT |			\
	 _TIF_SYSCALL_TRACEPOINT | ARCH_SYSCALL_EXIT_WORK)


#ifndef ARCH_EXIT_TO_USER_MODE_WORK
# define ARCH_EXIT_TO_USER_MODE_WORK		(0)
#endif

#define EXIT_TO_USER_MODE_WORK						\
	(_TIF_SIGPENDING | _TIF_NOTIFY_RESUME | _TIF_UPROBE |		\
	 _TIF_NEED_RESCHED | _TIF_PATCH_PENDING |			\
	 ARCH_EXIT_TO_USER_MODE_WORK)


static __always_inline void arch_check_user_regs(struct pt_regs *regs);

#ifndef arch_check_user_regs
static __always_inline void arch_check_user_regs(struct pt_regs *regs) {}
#endif


static inline __must_check int arch_syscall_enter_tracehook(struct pt_regs *regs);

#ifndef arch_syscall_enter_tracehook
static inline __must_check int arch_syscall_enter_tracehook(struct pt_regs *regs)
{
	return tracehook_report_syscall_entry(regs);
}
#endif


void syscall_enter_from_user_mode_prepare(struct pt_regs *regs);


long syscall_enter_from_user_mode_work(struct pt_regs *regs, long syscall);


long syscall_enter_from_user_mode(struct pt_regs *regs, long syscall);


static inline void local_irq_enable_exit_to_user(unsigned long ti_work);

#ifndef local_irq_enable_exit_to_user
static inline void local_irq_enable_exit_to_user(unsigned long ti_work)
{
	local_irq_enable();
}
#endif


static inline void local_irq_disable_exit_to_user(void);

#ifndef local_irq_disable_exit_to_user
static inline void local_irq_disable_exit_to_user(void)
{
	local_irq_disable();
}
#endif


static inline void arch_exit_to_user_mode_work(struct pt_regs *regs,
					       unsigned long ti_work);

#ifndef arch_exit_to_user_mode_work
static inline void arch_exit_to_user_mode_work(struct pt_regs *regs,
					       unsigned long ti_work)
{
}
#endif


static inline void arch_exit_to_user_mode_prepare(struct pt_regs *regs,
						  unsigned long ti_work);

#ifndef arch_exit_to_user_mode_prepare
static inline void arch_exit_to_user_mode_prepare(struct pt_regs *regs,
						  unsigned long ti_work)
{
}
#endif


static __always_inline void arch_exit_to_user_mode(void);

#ifndef arch_exit_to_user_mode
static __always_inline void arch_exit_to_user_mode(void) { }
#endif


void arch_do_signal(struct pt_regs *regs);


static inline void arch_syscall_exit_tracehook(struct pt_regs *regs, bool step);

#ifndef arch_syscall_exit_tracehook
static inline void arch_syscall_exit_tracehook(struct pt_regs *regs, bool step)
{
	tracehook_report_syscall_exit(regs, step);
}
#endif


void syscall_exit_to_user_mode(struct pt_regs *regs);


void irqentry_enter_from_user_mode(struct pt_regs *regs);


void irqentry_exit_to_user_mode(struct pt_regs *regs);

#ifndef irqentry_state

typedef struct irqentry_state {
	union {
		bool	exit_rcu;
		bool	lockdep;
	};
} irqentry_state_t;
#endif


irqentry_state_t noinstr irqentry_enter(struct pt_regs *regs);


void irqentry_exit_cond_resched(void);


void noinstr irqentry_exit(struct pt_regs *regs, irqentry_state_t state);


irqentry_state_t noinstr irqentry_nmi_enter(struct pt_regs *regs);


void noinstr irqentry_nmi_exit(struct pt_regs *regs, irqentry_state_t irq_state);

#endif
