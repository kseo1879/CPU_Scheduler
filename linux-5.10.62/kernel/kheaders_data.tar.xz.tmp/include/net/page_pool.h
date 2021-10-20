/* SPDX-License-Identifier: GPL-2.0
 *
 * page_pool.h
 *	Author:	Jesper Dangaard Brouer <netoptimizer@brouer.com>
 *	Copyright (C) 2016 Red Hat, Inc.
 */


#ifndef _NET_PAGE_POOL_H
#define _NET_PAGE_POOL_H

#include <linux/mm.h> 
#include <linux/ptr_ring.h>
#include <linux/dma-direction.h>

#define PP_FLAG_DMA_MAP		BIT(0) 
#define PP_FLAG_DMA_SYNC_DEV	BIT(1) 
#define PP_FLAG_ALL		(PP_FLAG_DMA_MAP | PP_FLAG_DMA_SYNC_DEV)


#define PP_ALLOC_CACHE_SIZE	128
#define PP_ALLOC_CACHE_REFILL	64
struct pp_alloc_cache {
	u32 count;
	void *cache[PP_ALLOC_CACHE_SIZE];
};

struct page_pool_params {
	unsigned int	flags;
	unsigned int	order;
	unsigned int	pool_size;
	int		nid;  
	struct device	*dev; 
	enum dma_data_direction dma_dir; 
	unsigned int	max_len; 
	unsigned int	offset;  
};

struct page_pool {
	struct page_pool_params p;

	struct delayed_work release_dw;
	void (*disconnect)(void *);
	unsigned long defer_start;
	unsigned long defer_warn;

	u32 pages_state_hold_cnt;

	
	struct pp_alloc_cache alloc ____cacheline_aligned_in_smp;

	
	struct ptr_ring ring;

	atomic_t pages_state_release_cnt;

	
	refcount_t user_cnt;

	u64 destroy_cnt;
};

struct page *page_pool_alloc_pages(struct page_pool *pool, gfp_t gfp);

static inline struct page *page_pool_dev_alloc_pages(struct page_pool *pool)
{
	gfp_t gfp = (GFP_ATOMIC | __GFP_NOWARN);

	return page_pool_alloc_pages(pool, gfp);
}


static
inline enum dma_data_direction page_pool_get_dma_dir(struct page_pool *pool)
{
	return pool->p.dma_dir;
}

struct page_pool *page_pool_create(const struct page_pool_params *params);

#ifdef CONFIG_PAGE_POOL
void page_pool_destroy(struct page_pool *pool);
void page_pool_use_xdp_mem(struct page_pool *pool, void (*disconnect)(void *));
void page_pool_release_page(struct page_pool *pool, struct page *page);
#else
static inline void page_pool_destroy(struct page_pool *pool)
{
}

static inline void page_pool_use_xdp_mem(struct page_pool *pool,
					 void (*disconnect)(void *))
{
}
static inline void page_pool_release_page(struct page_pool *pool,
					  struct page *page)
{
}
#endif

void page_pool_put_page(struct page_pool *pool, struct page *page,
			unsigned int dma_sync_size, bool allow_direct);


static inline void page_pool_put_full_page(struct page_pool *pool,
					   struct page *page, bool allow_direct)
{
	
#ifdef CONFIG_PAGE_POOL
	page_pool_put_page(pool, page, -1, allow_direct);
#endif
}


static inline void page_pool_recycle_direct(struct page_pool *pool,
					    struct page *page)
{
	page_pool_put_full_page(pool, page, true);
}

static inline dma_addr_t page_pool_get_dma_addr(struct page *page)
{
	dma_addr_t ret = page->dma_addr[0];
	if (sizeof(dma_addr_t) > sizeof(unsigned long))
		ret |= (dma_addr_t)page->dma_addr[1] << 16 << 16;
	return ret;
}

static inline void page_pool_set_dma_addr(struct page *page, dma_addr_t addr)
{
	page->dma_addr[0] = addr;
	if (sizeof(dma_addr_t) > sizeof(unsigned long))
		page->dma_addr[1] = upper_32_bits(addr);
}

static inline bool is_page_pool_compiled_in(void)
{
#ifdef CONFIG_PAGE_POOL
	return true;
#else
	return false;
#endif
}

static inline bool page_pool_put(struct page_pool *pool)
{
	return refcount_dec_and_test(&pool->user_cnt);
}


void page_pool_update_nid(struct page_pool *pool, int new_nid);
static inline void page_pool_nid_changed(struct page_pool *pool, int new_nid)
{
	if (unlikely(pool->p.nid != new_nid))
		page_pool_update_nid(pool, new_nid);
}
#endif 
