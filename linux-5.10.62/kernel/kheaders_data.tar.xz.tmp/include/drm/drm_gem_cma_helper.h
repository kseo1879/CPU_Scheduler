/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __DRM_GEM_CMA_HELPER_H__
#define __DRM_GEM_CMA_HELPER_H__

#include <drm/drm_file.h>
#include <drm/drm_ioctl.h>
#include <drm/drm_gem.h>

struct drm_mode_create_dumb;


struct drm_gem_cma_object {
	struct drm_gem_object base;
	dma_addr_t paddr;
	struct sg_table *sgt;

	
	void *vaddr;
};

#define to_drm_gem_cma_obj(gem_obj) \
	container_of(gem_obj, struct drm_gem_cma_object, base)

#ifndef CONFIG_MMU
#define DRM_GEM_CMA_UNMAPPED_AREA_FOPS \
	.get_unmapped_area	= drm_gem_cma_get_unmapped_area,
#else
#define DRM_GEM_CMA_UNMAPPED_AREA_FOPS
#endif


#define DEFINE_DRM_GEM_CMA_FOPS(name) \
	static const struct file_operations name = {\
		.owner		= THIS_MODULE,\
		.open		= drm_open,\
		.release	= drm_release,\
		.unlocked_ioctl	= drm_ioctl,\
		.compat_ioctl	= drm_compat_ioctl,\
		.poll		= drm_poll,\
		.read		= drm_read,\
		.llseek		= noop_llseek,\
		.mmap		= drm_gem_cma_mmap,\
		DRM_GEM_CMA_UNMAPPED_AREA_FOPS \
	}


void drm_gem_cma_free_object(struct drm_gem_object *gem_obj);


int drm_gem_cma_dumb_create_internal(struct drm_file *file_priv,
				     struct drm_device *drm,
				     struct drm_mode_create_dumb *args);


int drm_gem_cma_dumb_create(struct drm_file *file_priv,
			    struct drm_device *drm,
			    struct drm_mode_create_dumb *args);


int drm_gem_cma_mmap(struct file *filp, struct vm_area_struct *vma);


struct drm_gem_cma_object *drm_gem_cma_create(struct drm_device *drm,
					      size_t size);

extern const struct vm_operations_struct drm_gem_cma_vm_ops;

#ifndef CONFIG_MMU
unsigned long drm_gem_cma_get_unmapped_area(struct file *filp,
					    unsigned long addr,
					    unsigned long len,
					    unsigned long pgoff,
					    unsigned long flags);
#endif

void drm_gem_cma_print_info(struct drm_printer *p, unsigned int indent,
			    const struct drm_gem_object *obj);

struct sg_table *drm_gem_cma_prime_get_sg_table(struct drm_gem_object *obj);
struct drm_gem_object *
drm_gem_cma_prime_import_sg_table(struct drm_device *dev,
				  struct dma_buf_attachment *attach,
				  struct sg_table *sgt);
int drm_gem_cma_prime_mmap(struct drm_gem_object *obj,
			   struct vm_area_struct *vma);
void *drm_gem_cma_prime_vmap(struct drm_gem_object *obj);
void drm_gem_cma_prime_vunmap(struct drm_gem_object *obj, void *vaddr);

struct drm_gem_object *
drm_gem_cma_create_object_default_funcs(struct drm_device *dev, size_t size);


#define DRM_GEM_CMA_DRIVER_OPS_WITH_DUMB_CREATE(dumb_create_func) \
	.gem_create_object	= drm_gem_cma_create_object_default_funcs, \
	.dumb_create		= (dumb_create_func), \
	.prime_handle_to_fd	= drm_gem_prime_handle_to_fd, \
	.prime_fd_to_handle	= drm_gem_prime_fd_to_handle, \
	.gem_prime_import_sg_table = drm_gem_cma_prime_import_sg_table, \
	.gem_prime_mmap		= drm_gem_cma_prime_mmap


#define DRM_GEM_CMA_DRIVER_OPS \
	DRM_GEM_CMA_DRIVER_OPS_WITH_DUMB_CREATE(drm_gem_cma_dumb_create)


#define DRM_GEM_CMA_DRIVER_OPS_VMAP_WITH_DUMB_CREATE(dumb_create_func) \
	.gem_create_object	= drm_gem_cma_create_object_default_funcs, \
	.dumb_create		= dumb_create_func, \
	.prime_handle_to_fd	= drm_gem_prime_handle_to_fd, \
	.prime_fd_to_handle	= drm_gem_prime_fd_to_handle, \
	.gem_prime_import_sg_table = drm_gem_cma_prime_import_sg_table_vmap, \
	.gem_prime_mmap		= drm_gem_prime_mmap


#define DRM_GEM_CMA_DRIVER_OPS_VMAP \
	DRM_GEM_CMA_DRIVER_OPS_VMAP_WITH_DUMB_CREATE(drm_gem_cma_dumb_create)

struct drm_gem_object *
drm_gem_cma_prime_import_sg_table_vmap(struct drm_device *drm,
				       struct dma_buf_attachment *attach,
				       struct sg_table *sgt);

#endif 
