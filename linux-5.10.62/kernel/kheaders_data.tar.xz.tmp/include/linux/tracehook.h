/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _LINUX_TRACEHOOK_H
#define _LINUX_TRACEHOOK_H	1

#include <linux/sched.h>
#include <linux/ptrace.h>
#include <linux/security.h>
#include <linux/task_work.h>
#include <linux/memcontrol.h>
#include <linux/blk-cgroup.h>
struct linux_binprm;


static inline int ptrace_report_syscall(struct pt_regs *regs,
					unsigned long message)
{
	int ptrace = current->ptrace;

	if (!(ptrace & PT_PTRACED))
		return 0;

	current->ptrace_message = message;
	ptrace_notify(SIGTRAP | ((ptrace & PT_TRACESYSGOOD) ? 0x80 : 0));

	
	if (current->exit_code) {
		send_sig(current->exit_code, current, 1);
		current->exit_code = 0;
	}

	current->ptrace_message = 0;
	return fatal_signal_pending(current);
}


static inline __must_check int tracehook_report_syscall_entry(
	struct pt_regs *regs)
{
	return ptrace_report_syscall(regs, PTRACE_EVENTMSG_SYSCALL_ENTRY);
}


static inline void tracehook_report_syscall_exit(struct pt_regs *regs, int step)
{
	if (step)
		user_single_step_report(regs);
	else
		ptrace_report_syscall(regs, PTRACE_EVENTMSG_SYSCALL_EXIT);
}


static inline void tracehook_signal_handler(int stepping)
{
	if (stepping)
		ptrace_notify(SIGTRAP);
}


static inline void set_notify_resume(struct task_struct *task)
{
#ifdef TIF_NOTIFY_RESUME
	if (!test_and_set_tsk_thread_flag(task, TIF_NOTIFY_RESUME))
		kick_process(task);
#endif
}


static inline void tracehook_notify_resume(struct pt_regs *regs)
{
	clear_thread_flag(TIF_NOTIFY_RESUME);
	
	smp_mb__after_atomic();
	if (unlikely(current->task_works))
		task_work_run();

#ifdef CONFIG_KEYS_REQUEST_CACHE
	if (unlikely(current->cached_requested_key)) {
		key_put(current->cached_requested_key);
		current->cached_requested_key = NULL;
	}
#endif

	mem_cgroup_handle_over_high();
	blkcg_maybe_throttle_current();
}

#endif	
