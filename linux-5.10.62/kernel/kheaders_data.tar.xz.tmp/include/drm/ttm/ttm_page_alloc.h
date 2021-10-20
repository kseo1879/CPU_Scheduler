
#ifndef TTM_PAGE_ALLOC
#define TTM_PAGE_ALLOC

#include <drm/ttm/ttm_bo_driver.h>
#include <drm/ttm/ttm_memory.h>

struct device;


int ttm_page_alloc_init(struct ttm_mem_global *glob, unsigned max_pages);

void ttm_page_alloc_fini(void);


int ttm_pool_populate(struct ttm_tt *ttm, struct ttm_operation_ctx *ctx);


void ttm_pool_unpopulate(struct ttm_tt *ttm);


int ttm_populate_and_map_pages(struct device *dev, struct ttm_dma_tt *tt,
				struct ttm_operation_ctx *ctx);


void ttm_unmap_and_unpopulate_pages(struct device *dev, struct ttm_dma_tt *tt);


int ttm_page_alloc_debugfs(struct seq_file *m, void *data);

#if defined(CONFIG_DRM_TTM_DMA_PAGE_POOL)

int ttm_dma_page_alloc_init(struct ttm_mem_global *glob, unsigned max_pages);


void ttm_dma_page_alloc_fini(void);


int ttm_dma_page_alloc_debugfs(struct seq_file *m, void *data);

int ttm_dma_populate(struct ttm_dma_tt *ttm_dma, struct device *dev,
			struct ttm_operation_ctx *ctx);
void ttm_dma_unpopulate(struct ttm_dma_tt *ttm_dma, struct device *dev);

#else
static inline int ttm_dma_page_alloc_init(struct ttm_mem_global *glob,
					  unsigned max_pages)
{
	return -ENODEV;
}

static inline void ttm_dma_page_alloc_fini(void) { return; }

static inline int ttm_dma_page_alloc_debugfs(struct seq_file *m, void *data)
{
	return 0;
}
static inline int ttm_dma_populate(struct ttm_dma_tt *ttm_dma,
				struct device *dev,
				struct ttm_operation_ctx *ctx)
{
	return -ENOMEM;
}
static inline void ttm_dma_unpopulate(struct ttm_dma_tt *ttm_dma,
				      struct device *dev)
{
}
#endif

#endif
