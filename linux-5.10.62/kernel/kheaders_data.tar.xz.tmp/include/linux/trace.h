/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_TRACE_H
#define _LINUX_TRACE_H

#ifdef CONFIG_TRACING

#define TRACE_EXPORT_FUNCTION	BIT(0)
#define TRACE_EXPORT_EVENT	BIT(1)
#define TRACE_EXPORT_MARKER	BIT(2)


struct trace_export {
	struct trace_export __rcu	*next;
	void (*write)(struct trace_export *, const void *, unsigned int);
	int flags;
};

int register_ftrace_export(struct trace_export *export);
int unregister_ftrace_export(struct trace_export *export);

struct trace_array;

void trace_printk_init_buffers(void);
int trace_array_printk(struct trace_array *tr, unsigned long ip,
		const char *fmt, ...);
int trace_array_init_printk(struct trace_array *tr);
void trace_array_put(struct trace_array *tr);
struct trace_array *trace_array_get_by_name(const char *name);
int trace_array_destroy(struct trace_array *tr);
#endif	

#endif	
