/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_SECCOMP_H
#define _LINUX_SECCOMP_H

#include <uapi/linux/seccomp.h>

#define SECCOMP_FILTER_FLAG_MASK	(SECCOMP_FILTER_FLAG_TSYNC | \
					 SECCOMP_FILTER_FLAG_LOG | \
					 SECCOMP_FILTER_FLAG_SPEC_ALLOW | \
					 SECCOMP_FILTER_FLAG_NEW_LISTENER | \
					 SECCOMP_FILTER_FLAG_TSYNC_ESRCH)


#define SECCOMP_NOTIFY_ADDFD_SIZE_VER0 24
#define SECCOMP_NOTIFY_ADDFD_SIZE_LATEST SECCOMP_NOTIFY_ADDFD_SIZE_VER0

#ifdef CONFIG_SECCOMP

#include <linux/thread_info.h>
#include <linux/atomic.h>
#include <asm/seccomp.h>

struct seccomp_filter;

struct seccomp {
	int mode;
	atomic_t filter_count;
	struct seccomp_filter *filter;
};

#ifdef CONFIG_HAVE_ARCH_SECCOMP_FILTER
extern int __secure_computing(const struct seccomp_data *sd);
static inline int secure_computing(void)
{
	if (unlikely(test_thread_flag(TIF_SECCOMP)))
		return  __secure_computing(NULL);
	return 0;
}
#else
extern void secure_computing_strict(int this_syscall);
#endif

extern long prctl_get_seccomp(void);
extern long prctl_set_seccomp(unsigned long, void __user *);

static inline int seccomp_mode(struct seccomp *s)
{
	return s->mode;
}

#else 

#include <linux/errno.h>

struct seccomp { };
struct seccomp_filter { };
struct seccomp_data;

#ifdef CONFIG_HAVE_ARCH_SECCOMP_FILTER
static inline int secure_computing(void) { return 0; }
static inline int __secure_computing(const struct seccomp_data *sd) { return 0; }
#else
static inline void secure_computing_strict(int this_syscall) { return; }
#endif

static inline long prctl_get_seccomp(void)
{
	return -EINVAL;
}

static inline long prctl_set_seccomp(unsigned long arg2, char __user *arg3)
{
	return -EINVAL;
}

static inline int seccomp_mode(struct seccomp *s)
{
	return SECCOMP_MODE_DISABLED;
}
#endif 

#ifdef CONFIG_SECCOMP_FILTER
extern void seccomp_filter_release(struct task_struct *tsk);
extern void get_seccomp_filter(struct task_struct *tsk);
#else  
static inline void seccomp_filter_release(struct task_struct *tsk)
{
	return;
}
static inline void get_seccomp_filter(struct task_struct *tsk)
{
	return;
}
#endif 

#if defined(CONFIG_SECCOMP_FILTER) && defined(CONFIG_CHECKPOINT_RESTORE)
extern long seccomp_get_filter(struct task_struct *task,
			       unsigned long filter_off, void __user *data);
extern long seccomp_get_metadata(struct task_struct *task,
				 unsigned long filter_off, void __user *data);
#else
static inline long seccomp_get_filter(struct task_struct *task,
				      unsigned long n, void __user *data)
{
	return -EINVAL;
}
static inline long seccomp_get_metadata(struct task_struct *task,
					unsigned long filter_off,
					void __user *data)
{
	return -EINVAL;
}
#endif 
#endif 
