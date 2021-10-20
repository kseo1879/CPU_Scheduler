/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _LINUX_EVENTFD_H
#define _LINUX_EVENTFD_H

#include <linux/fcntl.h>
#include <linux/wait.h>
#include <linux/err.h>
#include <linux/percpu-defs.h>
#include <linux/percpu.h>


#define EFD_SEMAPHORE (1 << 0)
#define EFD_CLOEXEC O_CLOEXEC
#define EFD_NONBLOCK O_NONBLOCK

#define EFD_SHARED_FCNTL_FLAGS (O_CLOEXEC | O_NONBLOCK)
#define EFD_FLAGS_SET (EFD_SHARED_FCNTL_FLAGS | EFD_SEMAPHORE)

struct eventfd_ctx;
struct file;

#ifdef CONFIG_EVENTFD

void eventfd_ctx_put(struct eventfd_ctx *ctx);
struct file *eventfd_fget(int fd);
struct eventfd_ctx *eventfd_ctx_fdget(int fd);
struct eventfd_ctx *eventfd_ctx_fileget(struct file *file);
__u64 eventfd_signal(struct eventfd_ctx *ctx, __u64 n);
int eventfd_ctx_remove_wait_queue(struct eventfd_ctx *ctx, wait_queue_entry_t *wait,
				  __u64 *cnt);

DECLARE_PER_CPU(int, eventfd_wake_count);

static inline bool eventfd_signal_count(void)
{
	return this_cpu_read(eventfd_wake_count);
}

#else 



static inline struct eventfd_ctx *eventfd_ctx_fdget(int fd)
{
	return ERR_PTR(-ENOSYS);
}

static inline int eventfd_signal(struct eventfd_ctx *ctx, int n)
{
	return -ENOSYS;
}

static inline void eventfd_ctx_put(struct eventfd_ctx *ctx)
{

}

static inline int eventfd_ctx_remove_wait_queue(struct eventfd_ctx *ctx,
						wait_queue_entry_t *wait, __u64 *cnt)
{
	return -ENOSYS;
}

static inline bool eventfd_signal_count(void)
{
	return false;
}

#endif

#endif 

