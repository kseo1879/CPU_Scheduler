/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _LINUX_SCS_H
#define _LINUX_SCS_H

#include <linux/gfp.h>
#include <linux/poison.h>
#include <linux/sched.h>
#include <linux/sizes.h>

#ifdef CONFIG_SHADOW_CALL_STACK


#define SCS_SIZE		SZ_1K
#define GFP_SCS			(GFP_KERNEL | __GFP_ZERO)


#define SCS_END_MAGIC		(0x5f6UL + POISON_POINTER_DELTA)


#define DEFINE_SCS(name)						\
	DEFINE_PER_CPU(unsigned long [SCS_SIZE/sizeof(long)], name)	\

#define task_scs(tsk)		(task_thread_info(tsk)->scs_base)
#define task_scs_sp(tsk)	(task_thread_info(tsk)->scs_sp)

void scs_init(void);
int scs_prepare(struct task_struct *tsk, int node);
void scs_release(struct task_struct *tsk);

static inline void scs_task_reset(struct task_struct *tsk)
{
	
	task_scs_sp(tsk) = task_scs(tsk);
}

static inline unsigned long *__scs_magic(void *s)
{
	return (unsigned long *)(s + SCS_SIZE) - 1;
}

static inline bool task_scs_end_corrupted(struct task_struct *tsk)
{
	unsigned long *magic = __scs_magic(task_scs(tsk));
	unsigned long sz = task_scs_sp(tsk) - task_scs(tsk);

	return sz >= SCS_SIZE - 1 || READ_ONCE_NOCHECK(*magic) != SCS_END_MAGIC;
}

#else 

static inline void scs_init(void) {}
static inline void scs_task_reset(struct task_struct *tsk) {}
static inline int scs_prepare(struct task_struct *tsk, int node) { return 0; }
static inline void scs_release(struct task_struct *tsk) {}
static inline bool task_scs_end_corrupted(struct task_struct *tsk) { return false; }

#endif 

#endif 
