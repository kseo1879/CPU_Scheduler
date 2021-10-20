


#ifndef _TTM_BO_API_H_
#define _TTM_BO_API_H_

#include <drm/drm_gem.h>
#include <drm/drm_hashtab.h>
#include <drm/drm_vma_manager.h>
#include <linux/kref.h>
#include <linux/list.h>
#include <linux/wait.h>
#include <linux/mutex.h>
#include <linux/mm.h>
#include <linux/bitmap.h>
#include <linux/dma-resv.h>

#include "ttm_resource.h"

struct ttm_bo_global;

struct ttm_bo_device;

struct drm_mm_node;

struct ttm_placement;

struct ttm_place;

struct ttm_lru_bulk_move;



enum ttm_bo_type {
	ttm_bo_type_device,
	ttm_bo_type_kernel,
	ttm_bo_type_sg
};

struct ttm_tt;



struct ttm_buffer_object {
	struct drm_gem_object base;

	

	struct ttm_bo_device *bdev;
	enum ttm_bo_type type;
	void (*destroy) (struct ttm_buffer_object *);
	unsigned long num_pages;
	size_t acc_size;

	
	struct kref kref;

	

	struct ttm_resource mem;
	struct file *persistent_swap_storage;
	struct ttm_tt *ttm;
	bool deleted;

	

	struct list_head lru;
	struct list_head ddestroy;
	struct list_head swap;

	

	struct dma_fence *moving;
	unsigned priority;

	

	struct sg_table *sg;
};



#define TTM_BO_MAP_IOMEM_MASK 0x80
struct ttm_bo_kmap_obj {
	void *virtual;
	struct page *page;
	enum {
		ttm_bo_map_iomap        = 1 | TTM_BO_MAP_IOMEM_MASK,
		ttm_bo_map_vmap         = 2,
		ttm_bo_map_kmap         = 3,
		ttm_bo_map_premapped    = 4 | TTM_BO_MAP_IOMEM_MASK,
	} bo_kmap_type;
	struct ttm_buffer_object *bo;
};


struct ttm_operation_ctx {
	bool interruptible;
	bool no_wait_gpu;
	struct dma_resv *resv;
	uint64_t bytes_moved;
	uint32_t flags;
};


#define TTM_OPT_FLAG_ALLOW_RES_EVICT		0x1

#define TTM_OPT_FLAG_FORCE_ALLOC		0x2


static inline void ttm_bo_get(struct ttm_buffer_object *bo)
{
	kref_get(&bo->kref);
}


static inline __must_check struct ttm_buffer_object *
ttm_bo_get_unless_zero(struct ttm_buffer_object *bo)
{
	if (!kref_get_unless_zero(&bo->kref))
		return NULL;
	return bo;
}


int ttm_bo_wait(struct ttm_buffer_object *bo, bool interruptible, bool no_wait);


bool ttm_bo_mem_compat(struct ttm_placement *placement, struct ttm_resource *mem,
		       uint32_t *new_flags);


int ttm_bo_validate(struct ttm_buffer_object *bo,
		    struct ttm_placement *placement,
		    struct ttm_operation_ctx *ctx);


void ttm_bo_put(struct ttm_buffer_object *bo);


void ttm_bo_move_to_lru_tail(struct ttm_buffer_object *bo,
			     struct ttm_lru_bulk_move *bulk);


void ttm_bo_bulk_move_lru_tail(struct ttm_lru_bulk_move *bulk);


int ttm_bo_lock_delayed_workqueue(struct ttm_bo_device *bdev);


void ttm_bo_unlock_delayed_workqueue(struct ttm_bo_device *bdev, int resched);


bool ttm_bo_eviction_valuable(struct ttm_buffer_object *bo,
			      const struct ttm_place *place);

size_t ttm_bo_dma_acc_size(struct ttm_bo_device *bdev,
			   unsigned long bo_size,
			   unsigned struct_size);



int ttm_bo_init_reserved(struct ttm_bo_device *bdev,
			 struct ttm_buffer_object *bo,
			 unsigned long size,
			 enum ttm_bo_type type,
			 struct ttm_placement *placement,
			 uint32_t page_alignment,
			 struct ttm_operation_ctx *ctx,
			 size_t acc_size,
			 struct sg_table *sg,
			 struct dma_resv *resv,
			 void (*destroy) (struct ttm_buffer_object *));


int ttm_bo_init(struct ttm_bo_device *bdev, struct ttm_buffer_object *bo,
		unsigned long size, enum ttm_bo_type type,
		struct ttm_placement *placement,
		uint32_t page_alignment, bool interrubtible, size_t acc_size,
		struct sg_table *sg, struct dma_resv *resv,
		void (*destroy) (struct ttm_buffer_object *));


int ttm_bo_create(struct ttm_bo_device *bdev, unsigned long size,
		  enum ttm_bo_type type, struct ttm_placement *placement,
		  uint32_t page_alignment, bool interruptible,
		  struct ttm_buffer_object **p_bo);


int ttm_bo_evict_mm(struct ttm_bo_device *bdev, unsigned mem_type);


static inline void *ttm_kmap_obj_virtual(struct ttm_bo_kmap_obj *map,
					 bool *is_iomem)
{
	*is_iomem = !!(map->bo_kmap_type & TTM_BO_MAP_IOMEM_MASK);
	return map->virtual;
}


int ttm_bo_kmap(struct ttm_buffer_object *bo, unsigned long start_page,
		unsigned long num_pages, struct ttm_bo_kmap_obj *map);


void ttm_bo_kunmap(struct ttm_bo_kmap_obj *map);


int ttm_bo_mmap_obj(struct vm_area_struct *vma, struct ttm_buffer_object *bo);


int ttm_bo_mmap(struct file *filp, struct vm_area_struct *vma,
		struct ttm_bo_device *bdev);


ssize_t ttm_bo_io(struct ttm_bo_device *bdev, struct file *filp,
		  const char __user *wbuf, char __user *rbuf,
		  size_t count, loff_t *f_pos, bool write);

int ttm_bo_swapout(struct ttm_bo_global *glob,
			struct ttm_operation_ctx *ctx);
void ttm_bo_swapout_all(void);


static inline bool ttm_bo_uses_embedded_gem_object(struct ttm_buffer_object *bo)
{
	return bo->base.dev != NULL;
}

int ttm_mem_evict_first(struct ttm_bo_device *bdev,
			struct ttm_resource_manager *man,
			const struct ttm_place *place,
			struct ttm_operation_ctx *ctx,
			struct ww_acquire_ctx *ticket);


#define TTM_BO_VM_NUM_PREFAULT 16

vm_fault_t ttm_bo_vm_reserve(struct ttm_buffer_object *bo,
			     struct vm_fault *vmf);

vm_fault_t ttm_bo_vm_fault_reserved(struct vm_fault *vmf,
				    pgprot_t prot,
				    pgoff_t num_prefault,
				    pgoff_t fault_page_size);

vm_fault_t ttm_bo_vm_fault(struct vm_fault *vmf);

void ttm_bo_vm_open(struct vm_area_struct *vma);

void ttm_bo_vm_close(struct vm_area_struct *vma);

int ttm_bo_vm_access(struct vm_area_struct *vma, unsigned long addr,
		     void *buf, int len, int write);

#endif
