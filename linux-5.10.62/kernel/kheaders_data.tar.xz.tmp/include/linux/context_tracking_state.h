/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_CONTEXT_TRACKING_STATE_H
#define _LINUX_CONTEXT_TRACKING_STATE_H

#include <linux/percpu.h>
#include <linux/static_key.h>

struct context_tracking {
	
	bool active;
	int recursion;
	enum ctx_state {
		CONTEXT_DISABLED = -1,	
		CONTEXT_KERNEL = 0,
		CONTEXT_USER,
		CONTEXT_GUEST,
	} state;
};

#ifdef CONFIG_CONTEXT_TRACKING
extern struct static_key_false context_tracking_key;
DECLARE_PER_CPU(struct context_tracking, context_tracking);

static __always_inline bool context_tracking_enabled(void)
{
	return static_branch_unlikely(&context_tracking_key);
}

static __always_inline bool context_tracking_enabled_cpu(int cpu)
{
	return context_tracking_enabled() && per_cpu(context_tracking.active, cpu);
}

static inline bool context_tracking_enabled_this_cpu(void)
{
	return context_tracking_enabled() && __this_cpu_read(context_tracking.active);
}

static __always_inline bool context_tracking_in_user(void)
{
	return __this_cpu_read(context_tracking.state) == CONTEXT_USER;
}
#else
static inline bool context_tracking_in_user(void) { return false; }
static inline bool context_tracking_enabled(void) { return false; }
static inline bool context_tracking_enabled_cpu(int cpu) { return false; }
static inline bool context_tracking_enabled_this_cpu(void) { return false; }
#endif 

#endif
