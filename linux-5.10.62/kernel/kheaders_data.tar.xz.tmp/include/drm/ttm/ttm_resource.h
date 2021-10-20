

#ifndef _TTM_RESOURCE_H_
#define _TTM_RESOURCE_H_

#include <linux/types.h>
#include <linux/mutex.h>
#include <linux/dma-fence.h>
#include <drm/drm_print.h>

#define TTM_MAX_BO_PRIORITY	4U

struct ttm_bo_device;
struct ttm_resource_manager;
struct ttm_resource;
struct ttm_place;
struct ttm_buffer_object;

struct ttm_resource_manager_func {
	
	int  (*alloc)(struct ttm_resource_manager *man,
		      struct ttm_buffer_object *bo,
		      const struct ttm_place *place,
		      struct ttm_resource *mem);

	
	void (*free)(struct ttm_resource_manager *man,
		     struct ttm_resource *mem);

	
	void (*debug)(struct ttm_resource_manager *man,
		      struct drm_printer *printer);
};


struct ttm_resource_manager {
	
	bool use_type;
	bool use_tt;
	uint64_t size;
	const struct ttm_resource_manager_func *func;
	spinlock_t move_lock;

	

	struct list_head lru[TTM_MAX_BO_PRIORITY];

	
	struct dma_fence *move;
};


struct ttm_bus_placement {
	void		*addr;
	phys_addr_t	offset;
	bool		is_iomem;
};


struct ttm_resource {
	void *mm_node;
	unsigned long start;
	unsigned long size;
	unsigned long num_pages;
	uint32_t page_alignment;
	uint32_t mem_type;
	uint32_t placement;
	struct ttm_bus_placement bus;
};


static inline void
ttm_resource_manager_set_used(struct ttm_resource_manager *man, bool used)
{
	man->use_type = used;
}


static inline bool ttm_resource_manager_used(struct ttm_resource_manager *man)
{
	return man->use_type;
}


static inline void
ttm_resource_manager_cleanup(struct ttm_resource_manager *man)
{
	dma_fence_put(man->move);
	man->move = NULL;
}

int ttm_resource_alloc(struct ttm_buffer_object *bo,
		       const struct ttm_place *place,
		       struct ttm_resource *res);
void ttm_resource_free(struct ttm_buffer_object *bo, struct ttm_resource *res);

void ttm_resource_manager_init(struct ttm_resource_manager *man,
			       unsigned long p_size);

int ttm_resource_manager_force_list_clean(struct ttm_bo_device *bdev,
					  struct ttm_resource_manager *man);

void ttm_resource_manager_debug(struct ttm_resource_manager *man,
				struct drm_printer *p);

#endif
