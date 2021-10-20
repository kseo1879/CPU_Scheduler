
#ifndef _TTM_TT_H_
#define _TTM_TT_H_

#include <linux/types.h>

struct ttm_tt;
struct ttm_resource;
struct ttm_buffer_object;
struct ttm_operation_ctx;

#define TTM_PAGE_FLAG_WRITE           (1 << 3)
#define TTM_PAGE_FLAG_SWAPPED         (1 << 4)
#define TTM_PAGE_FLAG_PERSISTENT_SWAP (1 << 5)
#define TTM_PAGE_FLAG_ZERO_ALLOC      (1 << 6)
#define TTM_PAGE_FLAG_DMA32           (1 << 7)
#define TTM_PAGE_FLAG_SG              (1 << 8)
#define TTM_PAGE_FLAG_NO_RETRY	      (1 << 9)

#define TTM_PAGE_FLAG_PRIV_POPULATED  (1 << 31)

enum ttm_caching_state {
	tt_uncached,
	tt_wc,
	tt_cached
};


struct ttm_tt {
	struct page **pages;
	uint32_t page_flags;
	unsigned long num_pages;
	struct sg_table *sg; 
	struct file *swap_storage;
	enum ttm_caching_state caching_state;
};

static inline bool ttm_tt_is_populated(struct ttm_tt *tt)
{
	return tt->page_flags & TTM_PAGE_FLAG_PRIV_POPULATED;
}

static inline void ttm_tt_set_unpopulated(struct ttm_tt *tt)
{
	tt->page_flags &= ~TTM_PAGE_FLAG_PRIV_POPULATED;
}

static inline void ttm_tt_set_populated(struct ttm_tt *tt)
{
	tt->page_flags |= TTM_PAGE_FLAG_PRIV_POPULATED;
}


struct ttm_dma_tt {
	struct ttm_tt ttm;
	dma_addr_t *dma_address;
	struct list_head pages_list;
};


int ttm_tt_create(struct ttm_buffer_object *bo, bool zero_alloc);


int ttm_tt_init(struct ttm_tt *ttm, struct ttm_buffer_object *bo,
		uint32_t page_flags);
int ttm_dma_tt_init(struct ttm_dma_tt *ttm_dma, struct ttm_buffer_object *bo,
		    uint32_t page_flags);
int ttm_sg_tt_init(struct ttm_dma_tt *ttm_dma, struct ttm_buffer_object *bo,
		   uint32_t page_flags);


void ttm_tt_fini(struct ttm_tt *ttm);
void ttm_dma_tt_fini(struct ttm_dma_tt *ttm_dma);


void ttm_tt_destroy(struct ttm_bo_device *bdev, struct ttm_tt *ttm);


void ttm_tt_destroy_common(struct ttm_bo_device *bdev, struct ttm_tt *ttm);


int ttm_tt_swapin(struct ttm_tt *ttm);


int ttm_tt_set_placement_caching(struct ttm_tt *ttm, uint32_t placement);
int ttm_tt_swapout(struct ttm_bo_device *bdev, struct ttm_tt *ttm, struct file *persistent_swap_storage);


int ttm_tt_populate(struct ttm_bo_device *bdev, struct ttm_tt *ttm, struct ttm_operation_ctx *ctx);


void ttm_tt_unpopulate(struct ttm_bo_device *bdev, struct ttm_tt *ttm);

#if IS_ENABLED(CONFIG_AGP)
#include <linux/agp_backend.h>


struct ttm_tt *ttm_agp_tt_create(struct ttm_buffer_object *bo,
				 struct agp_bridge_data *bridge,
				 uint32_t page_flags);
int ttm_agp_bind(struct ttm_tt *ttm, struct ttm_resource *bo_mem);
void ttm_agp_unbind(struct ttm_tt *ttm);
void ttm_agp_destroy(struct ttm_tt *ttm);
bool ttm_agp_is_bound(struct ttm_tt *ttm);
#endif

#endif
