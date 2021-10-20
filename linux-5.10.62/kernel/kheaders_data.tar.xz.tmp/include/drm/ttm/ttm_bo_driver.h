

#ifndef _TTM_BO_DRIVER_H_
#define _TTM_BO_DRIVER_H_

#include <drm/drm_mm.h>
#include <drm/drm_vma_manager.h>
#include <linux/workqueue.h>
#include <linux/fs.h>
#include <linux/spinlock.h>
#include <linux/dma-resv.h>

#include "ttm_bo_api.h"
#include "ttm_memory.h"
#include "ttm_module.h"
#include "ttm_placement.h"
#include "ttm_tt.h"



struct ttm_bo_driver {
	
	struct ttm_tt *(*ttm_tt_create)(struct ttm_buffer_object *bo,
					uint32_t page_flags);

	
	int (*ttm_tt_populate)(struct ttm_bo_device *bdev,
			       struct ttm_tt *ttm,
			       struct ttm_operation_ctx *ctx);

	
	void (*ttm_tt_unpopulate)(struct ttm_bo_device *bdev, struct ttm_tt *ttm);

	
	int (*ttm_tt_bind)(struct ttm_bo_device *bdev, struct ttm_tt *ttm, struct ttm_resource *bo_mem);

	
	void (*ttm_tt_unbind)(struct ttm_bo_device *bdev, struct ttm_tt *ttm);

	
	void (*ttm_tt_destroy)(struct ttm_bo_device *bdev, struct ttm_tt *ttm);

	
	bool (*eviction_valuable)(struct ttm_buffer_object *bo,
				  const struct ttm_place *place);
	

	void (*evict_flags)(struct ttm_buffer_object *bo,
			    struct ttm_placement *placement);

	
	int (*move)(struct ttm_buffer_object *bo, bool evict,
		    struct ttm_operation_ctx *ctx,
		    struct ttm_resource *new_mem);

	
	int (*verify_access)(struct ttm_buffer_object *bo,
			     struct file *filp);

	
	void (*move_notify)(struct ttm_buffer_object *bo,
			    bool evict,
			    struct ttm_resource *new_mem);
	
	int (*fault_reserve_notify)(struct ttm_buffer_object *bo);

	
	void (*swap_notify)(struct ttm_buffer_object *bo);

	
	int (*io_mem_reserve)(struct ttm_bo_device *bdev,
			      struct ttm_resource *mem);
	void (*io_mem_free)(struct ttm_bo_device *bdev,
			    struct ttm_resource *mem);

	
	unsigned long (*io_mem_pfn)(struct ttm_buffer_object *bo,
				    unsigned long page_offset);

	
	int (*access_memory)(struct ttm_buffer_object *bo, unsigned long offset,
			     void *buf, int len, int write);

	
	void (*del_from_lru_notify)(struct ttm_buffer_object *bo);

	
	void (*release_notify)(struct ttm_buffer_object *bo);
};



extern struct ttm_bo_global {

	

	struct kobject kobj;
	struct page *dummy_read_page;
	spinlock_t lru_lock;

	
	struct list_head device_list;

	
	struct list_head swap_lru[TTM_MAX_BO_PRIORITY];

	
	atomic_t bo_count;
} ttm_bo_glob;


#define TTM_NUM_MEM_TYPES 8



struct ttm_bo_device {

	
	struct list_head device_list;
	struct ttm_bo_driver *driver;
	
	struct ttm_resource_manager sysman;
	struct ttm_resource_manager *man_drv[TTM_NUM_MEM_TYPES];
	
	struct drm_vma_offset_manager *vma_manager;

	
	struct list_head ddestroy;

	

	struct address_space *dev_mapping;

	

	struct delayed_work wq;

	bool need_dma32;

	bool no_retry;
};

static inline struct ttm_resource_manager *ttm_manager_type(struct ttm_bo_device *bdev,
							    int mem_type)
{
	return bdev->man_drv[mem_type];
}

static inline void ttm_set_driver_manager(struct ttm_bo_device *bdev,
					  int type,
					  struct ttm_resource_manager *manager)
{
	bdev->man_drv[type] = manager;
}


struct ttm_lru_bulk_move_pos {
	struct ttm_buffer_object *first;
	struct ttm_buffer_object *last;
};


struct ttm_lru_bulk_move {
	struct ttm_lru_bulk_move_pos tt[TTM_MAX_BO_PRIORITY];
	struct ttm_lru_bulk_move_pos vram[TTM_MAX_BO_PRIORITY];
	struct ttm_lru_bulk_move_pos swap[TTM_MAX_BO_PRIORITY];
};




int ttm_bo_mem_space(struct ttm_buffer_object *bo,
		     struct ttm_placement *placement,
		     struct ttm_resource *mem,
		     struct ttm_operation_ctx *ctx);

int ttm_bo_device_release(struct ttm_bo_device *bdev);


int ttm_bo_device_init(struct ttm_bo_device *bdev,
		       struct ttm_bo_driver *driver,
		       struct address_space *mapping,
		       struct drm_vma_offset_manager *vma_manager,
		       bool need_dma32);


void ttm_bo_unmap_virtual(struct ttm_buffer_object *bo);


void ttm_bo_unmap_virtual_locked(struct ttm_buffer_object *bo);


static inline int ttm_bo_reserve(struct ttm_buffer_object *bo,
				 bool interruptible, bool no_wait,
				 struct ww_acquire_ctx *ticket)
{
	int ret = 0;

	if (no_wait) {
		bool success;
		if (WARN_ON(ticket))
			return -EBUSY;

		success = dma_resv_trylock(bo->base.resv);
		return success ? 0 : -EBUSY;
	}

	if (interruptible)
		ret = dma_resv_lock_interruptible(bo->base.resv, ticket);
	else
		ret = dma_resv_lock(bo->base.resv, ticket);
	if (ret == -EINTR)
		return -ERESTARTSYS;
	return ret;
}


static inline int ttm_bo_reserve_slowpath(struct ttm_buffer_object *bo,
					  bool interruptible,
					  struct ww_acquire_ctx *ticket)
{
	if (interruptible) {
		int ret = dma_resv_lock_slow_interruptible(bo->base.resv,
							   ticket);
		if (ret == -EINTR)
			ret = -ERESTARTSYS;
		return ret;
	}
	dma_resv_lock_slow(bo->base.resv, ticket);
	return 0;
}

static inline void ttm_bo_move_to_lru_tail_unlocked(struct ttm_buffer_object *bo)
{
	spin_lock(&ttm_bo_glob.lru_lock);
	ttm_bo_move_to_lru_tail(bo, NULL);
	spin_unlock(&ttm_bo_glob.lru_lock);
}

static inline void ttm_bo_assign_mem(struct ttm_buffer_object *bo,
				     struct ttm_resource *new_mem)
{
	bo->mem = *new_mem;
	new_mem->mm_node = NULL;
}


static inline void ttm_bo_move_null(struct ttm_buffer_object *bo,
				    struct ttm_resource *new_mem)
{
	struct ttm_resource *old_mem = &bo->mem;

	WARN_ON(old_mem->mm_node != NULL);
	ttm_bo_assign_mem(bo, new_mem);
}


static inline void ttm_bo_unreserve(struct ttm_buffer_object *bo)
{
	ttm_bo_move_to_lru_tail_unlocked(bo);
	dma_resv_unlock(bo->base.resv);
}



int ttm_mem_io_reserve(struct ttm_bo_device *bdev,
		       struct ttm_resource *mem);
void ttm_mem_io_free(struct ttm_bo_device *bdev,
		     struct ttm_resource *mem);


int ttm_bo_move_ttm(struct ttm_buffer_object *bo,
		    struct ttm_operation_ctx *ctx,
		    struct ttm_resource *new_mem);



int ttm_bo_move_memcpy(struct ttm_buffer_object *bo,
		       struct ttm_operation_ctx *ctx,
		       struct ttm_resource *new_mem);


void ttm_bo_free_old_node(struct ttm_buffer_object *bo);


int ttm_bo_move_accel_cleanup(struct ttm_buffer_object *bo,
			      struct dma_fence *fence, bool evict,
			      bool pipeline,
			      struct ttm_resource *new_mem);


int ttm_bo_pipeline_gutting(struct ttm_buffer_object *bo);


pgprot_t ttm_io_prot(uint32_t caching_flags, pgprot_t tmp);


int ttm_bo_tt_bind(struct ttm_buffer_object *bo, struct ttm_resource *mem);


void ttm_bo_tt_unbind(struct ttm_buffer_object *bo);


void ttm_bo_tt_destroy(struct ttm_buffer_object *bo);


int ttm_range_man_init(struct ttm_bo_device *bdev,
		       unsigned type, bool use_tt,
		       unsigned long p_size);


int ttm_range_man_fini(struct ttm_bo_device *bdev,
		       unsigned type);

#endif
