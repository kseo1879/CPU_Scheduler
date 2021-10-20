
#ifndef _LINUX_RESERVATION_H
#define _LINUX_RESERVATION_H

#include <linux/ww_mutex.h>
#include <linux/dma-fence.h>
#include <linux/slab.h>
#include <linux/seqlock.h>
#include <linux/rcupdate.h>

extern struct ww_class reservation_ww_class;


struct dma_resv_list {
	struct rcu_head rcu;
	u32 shared_count, shared_max;
	struct dma_fence __rcu *shared[];
};


struct dma_resv {
	struct ww_mutex lock;
	seqcount_ww_mutex_t seq;

	struct dma_fence __rcu *fence_excl;
	struct dma_resv_list __rcu *fence;
};

#define dma_resv_held(obj) lockdep_is_held(&(obj)->lock.base)
#define dma_resv_assert_held(obj) lockdep_assert_held(&(obj)->lock.base)


static inline struct dma_resv_list *dma_resv_get_list(struct dma_resv *obj)
{
	return rcu_dereference_protected(obj->fence,
					 dma_resv_held(obj));
}


static inline int dma_resv_lock(struct dma_resv *obj,
				struct ww_acquire_ctx *ctx)
{
	return ww_mutex_lock(&obj->lock, ctx);
}


static inline int dma_resv_lock_interruptible(struct dma_resv *obj,
					      struct ww_acquire_ctx *ctx)
{
	return ww_mutex_lock_interruptible(&obj->lock, ctx);
}


static inline void dma_resv_lock_slow(struct dma_resv *obj,
				      struct ww_acquire_ctx *ctx)
{
	ww_mutex_lock_slow(&obj->lock, ctx);
}


static inline int dma_resv_lock_slow_interruptible(struct dma_resv *obj,
						   struct ww_acquire_ctx *ctx)
{
	return ww_mutex_lock_slow_interruptible(&obj->lock, ctx);
}


static inline bool __must_check dma_resv_trylock(struct dma_resv *obj)
{
	return ww_mutex_trylock(&obj->lock);
}


static inline bool dma_resv_is_locked(struct dma_resv *obj)
{
	return ww_mutex_is_locked(&obj->lock);
}


static inline struct ww_acquire_ctx *dma_resv_locking_ctx(struct dma_resv *obj)
{
	return READ_ONCE(obj->lock.ctx);
}


static inline void dma_resv_unlock(struct dma_resv *obj)
{
#ifdef CONFIG_DEBUG_MUTEXES
	
	if (rcu_access_pointer(obj->fence)) {
		struct dma_resv_list *fence = dma_resv_get_list(obj);

		fence->shared_max = fence->shared_count;
	}
#endif
	ww_mutex_unlock(&obj->lock);
}


static inline struct dma_fence *
dma_resv_get_excl(struct dma_resv *obj)
{
	return rcu_dereference_protected(obj->fence_excl,
					 dma_resv_held(obj));
}


static inline struct dma_fence *
dma_resv_get_excl_rcu(struct dma_resv *obj)
{
	struct dma_fence *fence;

	if (!rcu_access_pointer(obj->fence_excl))
		return NULL;

	rcu_read_lock();
	fence = dma_fence_get_rcu_safe(&obj->fence_excl);
	rcu_read_unlock();

	return fence;
}

void dma_resv_init(struct dma_resv *obj);
void dma_resv_fini(struct dma_resv *obj);
int dma_resv_reserve_shared(struct dma_resv *obj, unsigned int num_fences);
void dma_resv_add_shared_fence(struct dma_resv *obj, struct dma_fence *fence);

void dma_resv_add_excl_fence(struct dma_resv *obj, struct dma_fence *fence);

int dma_resv_get_fences_rcu(struct dma_resv *obj,
			    struct dma_fence **pfence_excl,
			    unsigned *pshared_count,
			    struct dma_fence ***pshared);

int dma_resv_copy_fences(struct dma_resv *dst, struct dma_resv *src);

long dma_resv_wait_timeout_rcu(struct dma_resv *obj, bool wait_all, bool intr,
			       unsigned long timeout);

bool dma_resv_test_signaled_rcu(struct dma_resv *obj, bool test_all);

#endif 
