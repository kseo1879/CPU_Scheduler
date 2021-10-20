/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_PAGEMAP_H
#define _LINUX_PAGEMAP_H


#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/list.h>
#include <linux/highmem.h>
#include <linux/compiler.h>
#include <linux/uaccess.h>
#include <linux/gfp.h>
#include <linux/bitops.h>
#include <linux/hardirq.h> 
#include <linux/hugetlb_inline.h>

struct pagevec;


enum mapping_flags {
	AS_EIO		= 0,	
	AS_ENOSPC	= 1,	
	AS_MM_ALL_LOCKS	= 2,	
	AS_UNEVICTABLE	= 3,	
	AS_EXITING	= 4, 	
	
	AS_NO_WRITEBACK_TAGS = 5,
	AS_THP_SUPPORT = 6,	
};


static inline void mapping_set_error(struct address_space *mapping, int error)
{
	if (likely(!error))
		return;

	
	__filemap_set_wb_err(mapping, error);

	
	if (mapping->host)
		errseq_set(&mapping->host->i_sb->s_wb_err, error);

	
	if (error == -ENOSPC)
		set_bit(AS_ENOSPC, &mapping->flags);
	else
		set_bit(AS_EIO, &mapping->flags);
}

static inline void mapping_set_unevictable(struct address_space *mapping)
{
	set_bit(AS_UNEVICTABLE, &mapping->flags);
}

static inline void mapping_clear_unevictable(struct address_space *mapping)
{
	clear_bit(AS_UNEVICTABLE, &mapping->flags);
}

static inline bool mapping_unevictable(struct address_space *mapping)
{
	return mapping && test_bit(AS_UNEVICTABLE, &mapping->flags);
}

static inline void mapping_set_exiting(struct address_space *mapping)
{
	set_bit(AS_EXITING, &mapping->flags);
}

static inline int mapping_exiting(struct address_space *mapping)
{
	return test_bit(AS_EXITING, &mapping->flags);
}

static inline void mapping_set_no_writeback_tags(struct address_space *mapping)
{
	set_bit(AS_NO_WRITEBACK_TAGS, &mapping->flags);
}

static inline int mapping_use_writeback_tags(struct address_space *mapping)
{
	return !test_bit(AS_NO_WRITEBACK_TAGS, &mapping->flags);
}

static inline gfp_t mapping_gfp_mask(struct address_space * mapping)
{
	return mapping->gfp_mask;
}


static inline gfp_t mapping_gfp_constraint(struct address_space *mapping,
		gfp_t gfp_mask)
{
	return mapping_gfp_mask(mapping) & gfp_mask;
}


static inline void mapping_set_gfp_mask(struct address_space *m, gfp_t mask)
{
	m->gfp_mask = mask;
}

static inline bool mapping_thp_support(struct address_space *mapping)
{
	return test_bit(AS_THP_SUPPORT, &mapping->flags);
}

static inline int filemap_nr_thps(struct address_space *mapping)
{
#ifdef CONFIG_READ_ONLY_THP_FOR_FS
	return atomic_read(&mapping->nr_thps);
#else
	return 0;
#endif
}

static inline void filemap_nr_thps_inc(struct address_space *mapping)
{
#ifdef CONFIG_READ_ONLY_THP_FOR_FS
	if (!mapping_thp_support(mapping))
		atomic_inc(&mapping->nr_thps);
#else
	WARN_ON_ONCE(1);
#endif
}

static inline void filemap_nr_thps_dec(struct address_space *mapping)
{
#ifdef CONFIG_READ_ONLY_THP_FOR_FS
	if (!mapping_thp_support(mapping))
		atomic_dec(&mapping->nr_thps);
#else
	WARN_ON_ONCE(1);
#endif
}

void release_pages(struct page **pages, int nr);


static inline int __page_cache_add_speculative(struct page *page, int count)
{
#ifdef CONFIG_TINY_RCU
# ifdef CONFIG_PREEMPT_COUNT
	VM_BUG_ON(!in_atomic() && !irqs_disabled());
# endif
	
	VM_BUG_ON_PAGE(page_count(page) == 0, page);
	page_ref_add(page, count);

#else
	if (unlikely(!page_ref_add_unless(page, count, 0))) {
		
		return 0;
	}
#endif
	VM_BUG_ON_PAGE(PageTail(page), page);

	return 1;
}

static inline int page_cache_get_speculative(struct page *page)
{
	return __page_cache_add_speculative(page, 1);
}

static inline int page_cache_add_speculative(struct page *page, int count)
{
	return __page_cache_add_speculative(page, count);
}


static inline void attach_page_private(struct page *page, void *data)
{
	get_page(page);
	set_page_private(page, (unsigned long)data);
	SetPagePrivate(page);
}


static inline void *detach_page_private(struct page *page)
{
	void *data = (void *)page_private(page);

	if (!PagePrivate(page))
		return NULL;
	ClearPagePrivate(page);
	set_page_private(page, 0);
	put_page(page);

	return data;
}

#ifdef CONFIG_NUMA
extern struct page *__page_cache_alloc(gfp_t gfp);
#else
static inline struct page *__page_cache_alloc(gfp_t gfp)
{
	return alloc_pages(gfp, 0);
}
#endif

static inline struct page *page_cache_alloc(struct address_space *x)
{
	return __page_cache_alloc(mapping_gfp_mask(x));
}

static inline gfp_t readahead_gfp_mask(struct address_space *x)
{
	return mapping_gfp_mask(x) | __GFP_NORETRY | __GFP_NOWARN;
}

typedef int filler_t(void *, struct page *);

pgoff_t page_cache_next_miss(struct address_space *mapping,
			     pgoff_t index, unsigned long max_scan);
pgoff_t page_cache_prev_miss(struct address_space *mapping,
			     pgoff_t index, unsigned long max_scan);

#define FGP_ACCESSED		0x00000001
#define FGP_LOCK		0x00000002
#define FGP_CREAT		0x00000004
#define FGP_WRITE		0x00000008
#define FGP_NOFS		0x00000010
#define FGP_NOWAIT		0x00000020
#define FGP_FOR_MMAP		0x00000040
#define FGP_HEAD		0x00000080

struct page *pagecache_get_page(struct address_space *mapping, pgoff_t offset,
		int fgp_flags, gfp_t cache_gfp_mask);


static inline struct page *find_get_page(struct address_space *mapping,
					pgoff_t offset)
{
	return pagecache_get_page(mapping, offset, 0, 0);
}

static inline struct page *find_get_page_flags(struct address_space *mapping,
					pgoff_t offset, int fgp_flags)
{
	return pagecache_get_page(mapping, offset, fgp_flags, 0);
}


static inline struct page *find_lock_page(struct address_space *mapping,
					pgoff_t index)
{
	return pagecache_get_page(mapping, index, FGP_LOCK, 0);
}


static inline struct page *find_lock_head(struct address_space *mapping,
					pgoff_t index)
{
	return pagecache_get_page(mapping, index, FGP_LOCK | FGP_HEAD, 0);
}


static inline struct page *find_or_create_page(struct address_space *mapping,
					pgoff_t index, gfp_t gfp_mask)
{
	return pagecache_get_page(mapping, index,
					FGP_LOCK|FGP_ACCESSED|FGP_CREAT,
					gfp_mask);
}


static inline struct page *grab_cache_page_nowait(struct address_space *mapping,
				pgoff_t index)
{
	return pagecache_get_page(mapping, index,
			FGP_LOCK|FGP_CREAT|FGP_NOFS|FGP_NOWAIT,
			mapping_gfp_mask(mapping));
}


static inline bool thp_contains(struct page *head, pgoff_t index)
{
	
	if (PageHuge(head))
		return head->index == index;
	return page_index(head) == (index & ~(thp_nr_pages(head) - 1UL));
}


static inline struct page *find_subpage(struct page *head, pgoff_t index)
{
	
	if (PageHuge(head))
		return head;

	return head + (index & (thp_nr_pages(head) - 1));
}

unsigned find_get_entries(struct address_space *mapping, pgoff_t start,
			  unsigned int nr_entries, struct page **entries,
			  pgoff_t *indices);
unsigned find_get_pages_range(struct address_space *mapping, pgoff_t *start,
			pgoff_t end, unsigned int nr_pages,
			struct page **pages);
static inline unsigned find_get_pages(struct address_space *mapping,
			pgoff_t *start, unsigned int nr_pages,
			struct page **pages)
{
	return find_get_pages_range(mapping, start, (pgoff_t)-1, nr_pages,
				    pages);
}
unsigned find_get_pages_contig(struct address_space *mapping, pgoff_t start,
			       unsigned int nr_pages, struct page **pages);
unsigned find_get_pages_range_tag(struct address_space *mapping, pgoff_t *index,
			pgoff_t end, xa_mark_t tag, unsigned int nr_pages,
			struct page **pages);
static inline unsigned find_get_pages_tag(struct address_space *mapping,
			pgoff_t *index, xa_mark_t tag, unsigned int nr_pages,
			struct page **pages)
{
	return find_get_pages_range_tag(mapping, index, (pgoff_t)-1, tag,
					nr_pages, pages);
}

struct page *grab_cache_page_write_begin(struct address_space *mapping,
			pgoff_t index, unsigned flags);


static inline struct page *grab_cache_page(struct address_space *mapping,
								pgoff_t index)
{
	return find_or_create_page(mapping, index, mapping_gfp_mask(mapping));
}

extern struct page * read_cache_page(struct address_space *mapping,
				pgoff_t index, filler_t *filler, void *data);
extern struct page * read_cache_page_gfp(struct address_space *mapping,
				pgoff_t index, gfp_t gfp_mask);
extern int read_cache_pages(struct address_space *mapping,
		struct list_head *pages, filler_t *filler, void *data);

static inline struct page *read_mapping_page(struct address_space *mapping,
				pgoff_t index, void *data)
{
	return read_cache_page(mapping, index, NULL, data);
}


static inline pgoff_t page_to_index(struct page *page)
{
	pgoff_t pgoff;

	if (likely(!PageTransTail(page)))
		return page->index;

	
	pgoff = compound_head(page)->index;
	pgoff += page - compound_head(page);
	return pgoff;
}

extern pgoff_t hugetlb_basepage_index(struct page *page);


static inline pgoff_t page_to_pgoff(struct page *page)
{
	if (unlikely(PageHuge(page)))
		return hugetlb_basepage_index(page);
	return page_to_index(page);
}


static inline loff_t page_offset(struct page *page)
{
	return ((loff_t)page->index) << PAGE_SHIFT;
}

static inline loff_t page_file_offset(struct page *page)
{
	return ((loff_t)page_index(page)) << PAGE_SHIFT;
}

extern pgoff_t linear_hugepage_index(struct vm_area_struct *vma,
				     unsigned long address);

static inline pgoff_t linear_page_index(struct vm_area_struct *vma,
					unsigned long address)
{
	pgoff_t pgoff;
	if (unlikely(is_vm_hugetlb_page(vma)))
		return linear_hugepage_index(vma, address);
	pgoff = (address - vma->vm_start) >> PAGE_SHIFT;
	pgoff += vma->vm_pgoff;
	return pgoff;
}

struct wait_page_key {
	struct page *page;
	int bit_nr;
	int page_match;
};

struct wait_page_queue {
	struct page *page;
	int bit_nr;
	wait_queue_entry_t wait;
};

static inline bool wake_page_match(struct wait_page_queue *wait_page,
				  struct wait_page_key *key)
{
	if (wait_page->page != key->page)
	       return false;
	key->page_match = 1;

	if (wait_page->bit_nr != key->bit_nr)
		return false;

	return true;
}

extern void __lock_page(struct page *page);
extern int __lock_page_killable(struct page *page);
extern int __lock_page_async(struct page *page, struct wait_page_queue *wait);
extern int __lock_page_or_retry(struct page *page, struct mm_struct *mm,
				unsigned int flags);
extern void unlock_page(struct page *page);


static inline int trylock_page(struct page *page)
{
	page = compound_head(page);
	return (likely(!test_and_set_bit_lock(PG_locked, &page->flags)));
}


static inline void lock_page(struct page *page)
{
	might_sleep();
	if (!trylock_page(page))
		__lock_page(page);
}


static inline int lock_page_killable(struct page *page)
{
	might_sleep();
	if (!trylock_page(page))
		return __lock_page_killable(page);
	return 0;
}


static inline int lock_page_async(struct page *page,
				  struct wait_page_queue *wait)
{
	if (!trylock_page(page))
		return __lock_page_async(page, wait);
	return 0;
}


static inline int lock_page_or_retry(struct page *page, struct mm_struct *mm,
				     unsigned int flags)
{
	might_sleep();
	return trylock_page(page) || __lock_page_or_retry(page, mm, flags);
}


extern void wait_on_page_bit(struct page *page, int bit_nr);
extern int wait_on_page_bit_killable(struct page *page, int bit_nr);


static inline void wait_on_page_locked(struct page *page)
{
	if (PageLocked(page))
		wait_on_page_bit(compound_head(page), PG_locked);
}

static inline int wait_on_page_locked_killable(struct page *page)
{
	if (!PageLocked(page))
		return 0;
	return wait_on_page_bit_killable(compound_head(page), PG_locked);
}

extern void put_and_wait_on_page_locked(struct page *page);

void wait_on_page_writeback(struct page *page);
extern void end_page_writeback(struct page *page);
void wait_for_stable_page(struct page *page);

void page_endio(struct page *page, bool is_write, int err);


extern void add_page_wait_queue(struct page *page, wait_queue_entry_t *waiter);


static inline int fault_in_pages_writeable(char __user *uaddr, int size)
{
	char __user *end = uaddr + size - 1;

	if (unlikely(size == 0))
		return 0;

	if (unlikely(uaddr > end))
		return -EFAULT;
	
	do {
		if (unlikely(__put_user(0, uaddr) != 0))
			return -EFAULT;
		uaddr += PAGE_SIZE;
	} while (uaddr <= end);

	
	if (((unsigned long)uaddr & PAGE_MASK) ==
			((unsigned long)end & PAGE_MASK))
		return __put_user(0, end);

	return 0;
}

static inline int fault_in_pages_readable(const char __user *uaddr, int size)
{
	volatile char c;
	const char __user *end = uaddr + size - 1;

	if (unlikely(size == 0))
		return 0;

	if (unlikely(uaddr > end))
		return -EFAULT;

	do {
		if (unlikely(__get_user(c, uaddr) != 0))
			return -EFAULT;
		uaddr += PAGE_SIZE;
	} while (uaddr <= end);

	
	if (((unsigned long)uaddr & PAGE_MASK) ==
			((unsigned long)end & PAGE_MASK)) {
		return __get_user(c, end);
	}

	(void)c;
	return 0;
}

int add_to_page_cache_locked(struct page *page, struct address_space *mapping,
				pgoff_t index, gfp_t gfp_mask);
int add_to_page_cache_lru(struct page *page, struct address_space *mapping,
				pgoff_t index, gfp_t gfp_mask);
extern void delete_from_page_cache(struct page *page);
extern void __delete_from_page_cache(struct page *page, void *shadow);
int replace_page_cache_page(struct page *old, struct page *new, gfp_t gfp_mask);
void delete_from_page_cache_batch(struct address_space *mapping,
				  struct pagevec *pvec);


static inline int add_to_page_cache(struct page *page,
		struct address_space *mapping, pgoff_t offset, gfp_t gfp_mask)
{
	int error;

	__SetPageLocked(page);
	error = add_to_page_cache_locked(page, mapping, offset, gfp_mask);
	if (unlikely(error))
		__ClearPageLocked(page);
	return error;
}


struct readahead_control {
	struct file *file;
	struct address_space *mapping;

	pgoff_t _index;
	unsigned int _nr_pages;
	unsigned int _batch_count;
};

#define DEFINE_READAHEAD(rac, f, m, i)					\
	struct readahead_control rac = {				\
		.file = f,						\
		.mapping = m,						\
		._index = i,						\
	}

#define VM_READAHEAD_PAGES	(SZ_128K / PAGE_SIZE)

void page_cache_ra_unbounded(struct readahead_control *,
		unsigned long nr_to_read, unsigned long lookahead_count);
void page_cache_sync_ra(struct readahead_control *, struct file_ra_state *,
		unsigned long req_count);
void page_cache_async_ra(struct readahead_control *, struct file_ra_state *,
		struct page *, unsigned long req_count);


static inline
void page_cache_sync_readahead(struct address_space *mapping,
		struct file_ra_state *ra, struct file *file, pgoff_t index,
		unsigned long req_count)
{
	DEFINE_READAHEAD(ractl, file, mapping, index);
	page_cache_sync_ra(&ractl, ra, req_count);
}


static inline
void page_cache_async_readahead(struct address_space *mapping,
		struct file_ra_state *ra, struct file *file,
		struct page *page, pgoff_t index, unsigned long req_count)
{
	DEFINE_READAHEAD(ractl, file, mapping, index);
	page_cache_async_ra(&ractl, ra, page, req_count);
}


static inline struct page *readahead_page(struct readahead_control *rac)
{
	struct page *page;

	BUG_ON(rac->_batch_count > rac->_nr_pages);
	rac->_nr_pages -= rac->_batch_count;
	rac->_index += rac->_batch_count;

	if (!rac->_nr_pages) {
		rac->_batch_count = 0;
		return NULL;
	}

	page = xa_load(&rac->mapping->i_pages, rac->_index);
	VM_BUG_ON_PAGE(!PageLocked(page), page);
	rac->_batch_count = thp_nr_pages(page);

	return page;
}

static inline unsigned int __readahead_batch(struct readahead_control *rac,
		struct page **array, unsigned int array_sz)
{
	unsigned int i = 0;
	XA_STATE(xas, &rac->mapping->i_pages, 0);
	struct page *page;

	BUG_ON(rac->_batch_count > rac->_nr_pages);
	rac->_nr_pages -= rac->_batch_count;
	rac->_index += rac->_batch_count;
	rac->_batch_count = 0;

	xas_set(&xas, rac->_index);
	rcu_read_lock();
	xas_for_each(&xas, page, rac->_index + rac->_nr_pages - 1) {
		if (xas_retry(&xas, page))
			continue;
		VM_BUG_ON_PAGE(!PageLocked(page), page);
		VM_BUG_ON_PAGE(PageTail(page), page);
		array[i++] = page;
		rac->_batch_count += thp_nr_pages(page);

		
		if (PageHead(page))
			xas_set(&xas, rac->_index + rac->_batch_count);

		if (i == array_sz)
			break;
	}
	rcu_read_unlock();

	return i;
}


#define readahead_page_batch(rac, array)				\
	__readahead_batch(rac, array, ARRAY_SIZE(array))


static inline loff_t readahead_pos(struct readahead_control *rac)
{
	return (loff_t)rac->_index * PAGE_SIZE;
}


static inline loff_t readahead_length(struct readahead_control *rac)
{
	return (loff_t)rac->_nr_pages * PAGE_SIZE;
}


static inline pgoff_t readahead_index(struct readahead_control *rac)
{
	return rac->_index;
}


static inline unsigned int readahead_count(struct readahead_control *rac)
{
	return rac->_nr_pages;
}

static inline unsigned long dir_pages(struct inode *inode)
{
	return (unsigned long)(inode->i_size + PAGE_SIZE - 1) >>
			       PAGE_SHIFT;
}


static inline int page_mkwrite_check_truncate(struct page *page,
					      struct inode *inode)
{
	loff_t size = i_size_read(inode);
	pgoff_t index = size >> PAGE_SHIFT;
	int offset = offset_in_page(size);

	if (page->mapping != inode->i_mapping)
		return -EFAULT;

	
	if (page->index < index)
		return PAGE_SIZE;
	
	if (page->index > index || !offset)
		return -EFAULT;
	
	return offset;
}


static inline
unsigned int i_blocks_per_page(struct inode *inode, struct page *page)
{
	return thp_size(page) >> inode->i_blkbits;
}
#endif 
