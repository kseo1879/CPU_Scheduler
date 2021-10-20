/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_CONTEXT_TRACKING_H
#define _LINUX_CONTEXT_TRACKING_H

#include <linux/sched.h>
#include <linux/vtime.h>
#include <linux/context_tracking_state.h>
#include <linux/instrumentation.h>

#include <asm/ptrace.h>


#ifdef CONFIG_CONTEXT_TRACKING
extern void context_tracking_cpu_set(int cpu);


extern void __context_tracking_enter(enum ctx_state state);
extern void __context_tracking_exit(enum ctx_state state);

extern void context_tracking_enter(enum ctx_state state);
extern void context_tracking_exit(enum ctx_state state);
extern void context_tracking_user_enter(void);
extern void context_tracking_user_exit(void);

static inline void user_enter(void)
{
	if (context_tracking_enabled())
		context_tracking_enter(CONTEXT_USER);

}
static inline void user_exit(void)
{
	if (context_tracking_enabled())
		context_tracking_exit(CONTEXT_USER);
}


static __always_inline void user_enter_irqoff(void)
{
	if (context_tracking_enabled())
		__context_tracking_enter(CONTEXT_USER);

}
static __always_inline void user_exit_irqoff(void)
{
	if (context_tracking_enabled())
		__context_tracking_exit(CONTEXT_USER);
}

static inline enum ctx_state exception_enter(void)
{
	enum ctx_state prev_ctx;

	if (!context_tracking_enabled())
		return 0;

	prev_ctx = this_cpu_read(context_tracking.state);
	if (prev_ctx != CONTEXT_KERNEL)
		context_tracking_exit(prev_ctx);

	return prev_ctx;
}

static inline void exception_exit(enum ctx_state prev_ctx)
{
	if (context_tracking_enabled()) {
		if (prev_ctx != CONTEXT_KERNEL)
			context_tracking_enter(prev_ctx);
	}
}



static __always_inline enum ctx_state ct_state(void)
{
	return context_tracking_enabled() ?
		this_cpu_read(context_tracking.state) : CONTEXT_DISABLED;
}
#else
static inline void user_enter(void) { }
static inline void user_exit(void) { }
static inline void user_enter_irqoff(void) { }
static inline void user_exit_irqoff(void) { }
static inline enum ctx_state exception_enter(void) { return 0; }
static inline void exception_exit(enum ctx_state prev_ctx) { }
static inline enum ctx_state ct_state(void) { return CONTEXT_DISABLED; }
#endif 

#define CT_WARN_ON(cond) WARN_ON(context_tracking_enabled() && (cond))

#ifdef CONFIG_CONTEXT_TRACKING_FORCE
extern void context_tracking_init(void);
#else
static inline void context_tracking_init(void) { }
#endif 


#ifdef CONFIG_VIRT_CPU_ACCOUNTING_GEN

static __always_inline void guest_enter_irqoff(void)
{
	instrumentation_begin();
	if (vtime_accounting_enabled_this_cpu())
		vtime_guest_enter(current);
	else
		current->flags |= PF_VCPU;
	instrumentation_end();

	if (context_tracking_enabled())
		__context_tracking_enter(CONTEXT_GUEST);

	
	if (!context_tracking_enabled_this_cpu()) {
		instrumentation_begin();
		rcu_virt_note_context_switch(smp_processor_id());
		instrumentation_end();
	}
}

static __always_inline void context_tracking_guest_exit(void)
{
	if (context_tracking_enabled())
		__context_tracking_exit(CONTEXT_GUEST);
}

static __always_inline void vtime_account_guest_exit(void)
{
	if (vtime_accounting_enabled_this_cpu())
		vtime_guest_exit(current);
	else
		current->flags &= ~PF_VCPU;
}

static __always_inline void guest_exit_irqoff(void)
{
	context_tracking_guest_exit();

	instrumentation_begin();
	vtime_account_guest_exit();
	instrumentation_end();
}

#else
static __always_inline void guest_enter_irqoff(void)
{
	
	instrumentation_begin();
	vtime_account_kernel(current);
	current->flags |= PF_VCPU;
	rcu_virt_note_context_switch(smp_processor_id());
	instrumentation_end();
}

static __always_inline void context_tracking_guest_exit(void) { }

static __always_inline void vtime_account_guest_exit(void)
{
	vtime_account_kernel(current);
	current->flags &= ~PF_VCPU;
}

static __always_inline void guest_exit_irqoff(void)
{
	instrumentation_begin();
	
	vtime_account_guest_exit();
	instrumentation_end();
}
#endif 

static inline void guest_exit(void)
{
	unsigned long flags;

	local_irq_save(flags);
	guest_exit_irqoff();
	local_irq_restore(flags);
}

#endif
