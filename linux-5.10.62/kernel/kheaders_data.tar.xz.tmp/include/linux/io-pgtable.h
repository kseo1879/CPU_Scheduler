/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __IO_PGTABLE_H
#define __IO_PGTABLE_H

#include <linux/bitops.h>
#include <linux/iommu.h>


enum io_pgtable_fmt {
	ARM_32_LPAE_S1,
	ARM_32_LPAE_S2,
	ARM_64_LPAE_S1,
	ARM_64_LPAE_S2,
	ARM_V7S,
	ARM_MALI_LPAE,
	IO_PGTABLE_NUM_FMTS,
};


struct iommu_flush_ops {
	void (*tlb_flush_all)(void *cookie);
	void (*tlb_flush_walk)(unsigned long iova, size_t size, size_t granule,
			       void *cookie);
	void (*tlb_flush_leaf)(unsigned long iova, size_t size, size_t granule,
			       void *cookie);
	void (*tlb_add_page)(struct iommu_iotlb_gather *gather,
			     unsigned long iova, size_t granule, void *cookie);
};


struct io_pgtable_cfg {
	
	#define IO_PGTABLE_QUIRK_ARM_NS		BIT(0)
	#define IO_PGTABLE_QUIRK_NO_PERMS	BIT(1)
	#define IO_PGTABLE_QUIRK_TLBI_ON_MAP	BIT(2)
	#define IO_PGTABLE_QUIRK_ARM_MTK_EXT	BIT(3)
	#define IO_PGTABLE_QUIRK_NON_STRICT	BIT(4)
	#define IO_PGTABLE_QUIRK_ARM_TTBR1	BIT(5)
	unsigned long			quirks;
	unsigned long			pgsize_bitmap;
	unsigned int			ias;
	unsigned int			oas;
	bool				coherent_walk;
	const struct iommu_flush_ops	*tlb;
	struct device			*iommu_dev;

	
	union {
		struct {
			u64	ttbr;
			struct {
				u32	ips:3;
				u32	tg:2;
				u32	sh:2;
				u32	orgn:2;
				u32	irgn:2;
				u32	tsz:6;
			}	tcr;
			u64	mair;
		} arm_lpae_s1_cfg;

		struct {
			u64	vttbr;
			struct {
				u32	ps:3;
				u32	tg:2;
				u32	sh:2;
				u32	orgn:2;
				u32	irgn:2;
				u32	sl:2;
				u32	tsz:6;
			}	vtcr;
		} arm_lpae_s2_cfg;

		struct {
			u32	ttbr;
			u32	tcr;
			u32	nmrr;
			u32	prrr;
		} arm_v7s_cfg;

		struct {
			u64	transtab;
			u64	memattr;
		} arm_mali_lpae_cfg;
	};
};


struct io_pgtable_ops {
	int (*map)(struct io_pgtable_ops *ops, unsigned long iova,
		   phys_addr_t paddr, size_t size, int prot, gfp_t gfp);
	size_t (*unmap)(struct io_pgtable_ops *ops, unsigned long iova,
			size_t size, struct iommu_iotlb_gather *gather);
	phys_addr_t (*iova_to_phys)(struct io_pgtable_ops *ops,
				    unsigned long iova);
};


struct io_pgtable_ops *alloc_io_pgtable_ops(enum io_pgtable_fmt fmt,
					    struct io_pgtable_cfg *cfg,
					    void *cookie);


void free_io_pgtable_ops(struct io_pgtable_ops *ops);





struct io_pgtable {
	enum io_pgtable_fmt	fmt;
	void			*cookie;
	struct io_pgtable_cfg	cfg;
	struct io_pgtable_ops	ops;
};

#define io_pgtable_ops_to_pgtable(x) container_of((x), struct io_pgtable, ops)

static inline void io_pgtable_tlb_flush_all(struct io_pgtable *iop)
{
	iop->cfg.tlb->tlb_flush_all(iop->cookie);
}

static inline void
io_pgtable_tlb_flush_walk(struct io_pgtable *iop, unsigned long iova,
			  size_t size, size_t granule)
{
	iop->cfg.tlb->tlb_flush_walk(iova, size, granule, iop->cookie);
}

static inline void
io_pgtable_tlb_flush_leaf(struct io_pgtable *iop, unsigned long iova,
			  size_t size, size_t granule)
{
	iop->cfg.tlb->tlb_flush_leaf(iova, size, granule, iop->cookie);
}

static inline void
io_pgtable_tlb_add_page(struct io_pgtable *iop,
			struct iommu_iotlb_gather * gather, unsigned long iova,
			size_t granule)
{
	if (iop->cfg.tlb->tlb_add_page)
		iop->cfg.tlb->tlb_add_page(gather, iova, granule, iop->cookie);
}


struct io_pgtable_init_fns {
	struct io_pgtable *(*alloc)(struct io_pgtable_cfg *cfg, void *cookie);
	void (*free)(struct io_pgtable *iop);
};

extern struct io_pgtable_init_fns io_pgtable_arm_32_lpae_s1_init_fns;
extern struct io_pgtable_init_fns io_pgtable_arm_32_lpae_s2_init_fns;
extern struct io_pgtable_init_fns io_pgtable_arm_64_lpae_s1_init_fns;
extern struct io_pgtable_init_fns io_pgtable_arm_64_lpae_s2_init_fns;
extern struct io_pgtable_init_fns io_pgtable_arm_v7s_init_fns;
extern struct io_pgtable_init_fns io_pgtable_arm_mali_lpae_init_fns;

#endif 
