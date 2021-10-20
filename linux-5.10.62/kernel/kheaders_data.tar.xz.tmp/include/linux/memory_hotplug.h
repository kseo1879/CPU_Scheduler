/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_MEMORY_HOTPLUG_H
#define __LINUX_MEMORY_HOTPLUG_H

#include <linux/mmzone.h>
#include <linux/spinlock.h>
#include <linux/notifier.h>
#include <linux/bug.h>

struct page;
struct zone;
struct pglist_data;
struct mem_section;
struct memory_block;
struct resource;
struct vmem_altmap;

#ifdef CONFIG_MEMORY_HOTPLUG

#define pfn_to_online_page(pfn)					   \
({								   \
	struct page *___page = NULL;				   \
	unsigned long ___pfn = pfn;				   \
	unsigned long ___nr = pfn_to_section_nr(___pfn);	   \
								   \
	if (___nr < NR_MEM_SECTIONS && online_section_nr(___nr) && \
	    pfn_valid_within(___pfn))				   \
		___page = pfn_to_page(___pfn);			   \
	___page;						   \
})


enum {
	MEMORY_HOTPLUG_MIN_BOOTMEM_TYPE = 12,
	SECTION_INFO = MEMORY_HOTPLUG_MIN_BOOTMEM_TYPE,
	MIX_SECTION_INFO,
	NODE_INFO,
	MEMORY_HOTPLUG_MAX_BOOTMEM_TYPE = NODE_INFO,
};


enum {
	
	MMOP_OFFLINE = 0,
	
	MMOP_ONLINE,
	
	MMOP_ONLINE_KERNEL,
	
	MMOP_ONLINE_MOVABLE,
};


typedef int __bitwise mhp_t;


#define MHP_NONE		((__force mhp_t)0)

#define MEMHP_MERGE_RESOURCE	((__force mhp_t)BIT(0))


struct mhp_params {
	struct vmem_altmap *altmap;
	pgprot_t pgprot;
};


static inline unsigned zone_span_seqbegin(struct zone *zone)
{
	return read_seqbegin(&zone->span_seqlock);
}
static inline int zone_span_seqretry(struct zone *zone, unsigned iv)
{
	return read_seqretry(&zone->span_seqlock, iv);
}
static inline void zone_span_writelock(struct zone *zone)
{
	write_seqlock(&zone->span_seqlock);
}
static inline void zone_span_writeunlock(struct zone *zone)
{
	write_sequnlock(&zone->span_seqlock);
}
static inline void zone_seqlock_init(struct zone *zone)
{
	seqlock_init(&zone->span_seqlock);
}
extern int zone_grow_free_lists(struct zone *zone, unsigned long new_nr_pages);
extern int zone_grow_waitqueues(struct zone *zone, unsigned long nr_pages);
extern int add_one_highpage(struct page *page, int pfn, int bad_ppro);

extern int online_pages(unsigned long pfn, unsigned long nr_pages,
			int online_type, int nid);
extern struct zone *test_pages_in_a_zone(unsigned long start_pfn,
					 unsigned long end_pfn);
extern void __offline_isolated_pages(unsigned long start_pfn,
				     unsigned long end_pfn);

typedef void (*online_page_callback_t)(struct page *page, unsigned int order);

extern void generic_online_page(struct page *page, unsigned int order);
extern int set_online_page_callback(online_page_callback_t callback);
extern int restore_online_page_callback(online_page_callback_t callback);

extern int try_online_node(int nid);

extern int arch_add_memory(int nid, u64 start, u64 size,
			   struct mhp_params *params);
extern u64 max_mem_size;

extern int memhp_online_type_from_str(const char *str);


extern int memhp_default_online_type;

extern bool movable_node_enabled;
static inline bool movable_node_is_enabled(void)
{
	return movable_node_enabled;
}

extern void arch_remove_memory(int nid, u64 start, u64 size,
			       struct vmem_altmap *altmap);
extern void __remove_pages(unsigned long start_pfn, unsigned long nr_pages,
			   struct vmem_altmap *altmap);


extern int __add_pages(int nid, unsigned long start_pfn, unsigned long nr_pages,
		       struct mhp_params *params);

#ifndef CONFIG_ARCH_HAS_ADD_PAGES
static inline int add_pages(int nid, unsigned long start_pfn,
		unsigned long nr_pages, struct mhp_params *params)
{
	return __add_pages(nid, start_pfn, nr_pages, params);
}
#else 
int add_pages(int nid, unsigned long start_pfn, unsigned long nr_pages,
	      struct mhp_params *params);
#endif 

#ifdef CONFIG_HAVE_ARCH_NODEDATA_EXTENSION

extern pg_data_t *arch_alloc_nodedata(int nid);
extern void arch_free_nodedata(pg_data_t *pgdat);
extern void arch_refresh_nodedata(int nid, pg_data_t *pgdat);

#else 

#define arch_alloc_nodedata(nid)	generic_alloc_nodedata(nid)
#define arch_free_nodedata(pgdat)	generic_free_nodedata(pgdat)

#ifdef CONFIG_NUMA

#define generic_alloc_nodedata(nid)				\
({								\
	kzalloc(sizeof(pg_data_t), GFP_KERNEL);			\
})

#define generic_free_nodedata(pgdat)	kfree(pgdat)

extern pg_data_t *node_data[];
static inline void arch_refresh_nodedata(int nid, pg_data_t *pgdat)
{
	node_data[nid] = pgdat;
}

#else 


static inline pg_data_t *generic_alloc_nodedata(int nid)
{
	BUG();
	return NULL;
}
static inline void generic_free_nodedata(pg_data_t *pgdat)
{
}
static inline void arch_refresh_nodedata(int nid, pg_data_t *pgdat)
{
}
#endif 
#endif 

#ifdef CONFIG_HAVE_BOOTMEM_INFO_NODE
extern void __init register_page_bootmem_info_node(struct pglist_data *pgdat);
#else
static inline void register_page_bootmem_info_node(struct pglist_data *pgdat)
{
}
#endif
extern void put_page_bootmem(struct page *page);
extern void get_page_bootmem(unsigned long ingo, struct page *page,
			     unsigned long type);

void get_online_mems(void);
void put_online_mems(void);

void mem_hotplug_begin(void);
void mem_hotplug_done(void);

#else 
#define pfn_to_online_page(pfn)			\
({						\
	struct page *___page = NULL;		\
	if (pfn_valid(pfn))			\
		___page = pfn_to_page(pfn);	\
	___page;				\
 })

static inline unsigned zone_span_seqbegin(struct zone *zone)
{
	return 0;
}
static inline int zone_span_seqretry(struct zone *zone, unsigned iv)
{
	return 0;
}
static inline void zone_span_writelock(struct zone *zone) {}
static inline void zone_span_writeunlock(struct zone *zone) {}
static inline void zone_seqlock_init(struct zone *zone) {}

static inline void register_page_bootmem_info_node(struct pglist_data *pgdat)
{
}

static inline int try_online_node(int nid)
{
	return 0;
}

static inline void get_online_mems(void) {}
static inline void put_online_mems(void) {}

static inline void mem_hotplug_begin(void) {}
static inline void mem_hotplug_done(void) {}

static inline bool movable_node_is_enabled(void)
{
	return false;
}
#endif 

#if defined(CONFIG_MEMORY_HOTPLUG) || defined(CONFIG_DEFERRED_STRUCT_PAGE_INIT)

static inline
void pgdat_resize_lock(struct pglist_data *pgdat, unsigned long *flags)
{
	spin_lock_irqsave(&pgdat->node_size_lock, *flags);
}
static inline
void pgdat_resize_unlock(struct pglist_data *pgdat, unsigned long *flags)
{
	spin_unlock_irqrestore(&pgdat->node_size_lock, *flags);
}
static inline
void pgdat_resize_init(struct pglist_data *pgdat)
{
	spin_lock_init(&pgdat->node_size_lock);
}
#else 

static inline void pgdat_resize_lock(struct pglist_data *p, unsigned long *f) {}
static inline void pgdat_resize_unlock(struct pglist_data *p, unsigned long *f) {}
static inline void pgdat_resize_init(struct pglist_data *pgdat) {}
#endif 

#ifdef CONFIG_MEMORY_HOTREMOVE

extern void try_offline_node(int nid);
extern int offline_pages(unsigned long start_pfn, unsigned long nr_pages);
extern int remove_memory(int nid, u64 start, u64 size);
extern void __remove_memory(int nid, u64 start, u64 size);
extern int offline_and_remove_memory(int nid, u64 start, u64 size);

#else
static inline void try_offline_node(int nid) {}

static inline int offline_pages(unsigned long start_pfn, unsigned long nr_pages)
{
	return -EINVAL;
}

static inline int remove_memory(int nid, u64 start, u64 size)
{
	return -EBUSY;
}

static inline void __remove_memory(int nid, u64 start, u64 size) {}
#endif 

extern void set_zone_contiguous(struct zone *zone);
extern void clear_zone_contiguous(struct zone *zone);

#ifdef CONFIG_MEMORY_HOTPLUG
extern void __ref free_area_init_core_hotplug(int nid);
extern int __add_memory(int nid, u64 start, u64 size, mhp_t mhp_flags);
extern int add_memory(int nid, u64 start, u64 size, mhp_t mhp_flags);
extern int add_memory_resource(int nid, struct resource *resource,
			       mhp_t mhp_flags);
extern int add_memory_driver_managed(int nid, u64 start, u64 size,
				     const char *resource_name,
				     mhp_t mhp_flags);
extern void move_pfn_range_to_zone(struct zone *zone, unsigned long start_pfn,
				   unsigned long nr_pages,
				   struct vmem_altmap *altmap, int migratetype);
extern void remove_pfn_range_from_zone(struct zone *zone,
				       unsigned long start_pfn,
				       unsigned long nr_pages);
extern bool is_memblock_offlined(struct memory_block *mem);
extern int sparse_add_section(int nid, unsigned long pfn,
		unsigned long nr_pages, struct vmem_altmap *altmap);
extern void sparse_remove_section(struct mem_section *ms,
		unsigned long pfn, unsigned long nr_pages,
		unsigned long map_offset, struct vmem_altmap *altmap);
extern struct page *sparse_decode_mem_map(unsigned long coded_mem_map,
					  unsigned long pnum);
extern struct zone *zone_for_pfn_range(int online_type, int nid, unsigned start_pfn,
		unsigned long nr_pages);
#endif 

#endif 
