


#ifndef _TTM_PLACEMENT_H_
#define _TTM_PLACEMENT_H_

#include <linux/types.h>



#define TTM_PL_SYSTEM           0
#define TTM_PL_TT               1
#define TTM_PL_VRAM             2
#define TTM_PL_PRIV             3



#define TTM_PL_FLAG_CACHED      (1 << 16)
#define TTM_PL_FLAG_UNCACHED    (1 << 17)
#define TTM_PL_FLAG_WC          (1 << 18)
#define TTM_PL_FLAG_CONTIGUOUS  (1 << 19)
#define TTM_PL_FLAG_NO_EVICT    (1 << 21)
#define TTM_PL_FLAG_TOPDOWN     (1 << 22)

#define TTM_PL_MASK_CACHING     (TTM_PL_FLAG_CACHED | \
				 TTM_PL_FLAG_UNCACHED | \
				 TTM_PL_FLAG_WC)


struct ttm_place {
	unsigned	fpfn;
	unsigned	lpfn;
	uint32_t	mem_type;
	uint32_t	flags;
};


struct ttm_placement {
	unsigned		num_placement;
	const struct ttm_place	*placement;
	unsigned		num_busy_placement;
	const struct ttm_place	*busy_placement;
};

#endif
