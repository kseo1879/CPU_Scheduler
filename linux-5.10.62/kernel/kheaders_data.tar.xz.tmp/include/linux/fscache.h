/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _LINUX_FSCACHE_H
#define _LINUX_FSCACHE_H

#include <linux/fs.h>
#include <linux/list.h>
#include <linux/pagemap.h>
#include <linux/pagevec.h>
#include <linux/list_bl.h>

#if defined(CONFIG_FSCACHE) || defined(CONFIG_FSCACHE_MODULE)
#define fscache_available() (1)
#define fscache_cookie_valid(cookie) (cookie)
#else
#define fscache_available() (0)
#define fscache_cookie_valid(cookie) (0)
#endif



#define PageFsCache(page)		PagePrivate2((page))
#define SetPageFsCache(page)		SetPagePrivate2((page))
#define ClearPageFsCache(page)		ClearPagePrivate2((page))
#define TestSetPageFsCache(page)	TestSetPagePrivate2((page))
#define TestClearPageFsCache(page)	TestClearPagePrivate2((page))


#define FSCACHE_INDEX_DEADFILL_PATTERN 0x79

struct pagevec;
struct fscache_cache_tag;
struct fscache_cookie;
struct fscache_netfs;

typedef void (*fscache_rw_complete_t)(struct page *page,
				      void *context,
				      int error);


enum fscache_checkaux {
	FSCACHE_CHECKAUX_OKAY,		
	FSCACHE_CHECKAUX_NEEDS_UPDATE,	
	FSCACHE_CHECKAUX_OBSOLETE,	
};


struct fscache_cookie_def {
	
	char name[16];

	
	uint8_t type;
#define FSCACHE_COOKIE_TYPE_INDEX	0
#define FSCACHE_COOKIE_TYPE_DATAFILE	1

	
	struct fscache_cache_tag *(*select_cache)(
		const void *parent_netfs_data,
		const void *cookie_netfs_data);

	
	enum fscache_checkaux (*check_aux)(void *cookie_netfs_data,
					   const void *data,
					   uint16_t datalen,
					   loff_t object_size);

	
	void (*get_context)(void *cookie_netfs_data, void *context);

	
	void (*put_context)(void *cookie_netfs_data, void *context);

	
	void (*mark_page_cached)(void *cookie_netfs_data,
				 struct address_space *mapping,
				 struct page *page);
};


struct fscache_netfs {
	uint32_t			version;	
	const char			*name;		
	struct fscache_cookie		*primary_index;
};


struct fscache_cookie {
	atomic_t			usage;		
	atomic_t			n_children;	
	atomic_t			n_active;	
	spinlock_t			lock;
	spinlock_t			stores_lock;	
	struct hlist_head		backing_objects; 
	const struct fscache_cookie_def	*def;		
	struct fscache_cookie		*parent;	
	struct hlist_bl_node		hash_link;	
	void				*netfs_data;	
	struct radix_tree_root		stores;		
#define FSCACHE_COOKIE_PENDING_TAG	0		
#define FSCACHE_COOKIE_STORING_TAG	1		

	unsigned long			flags;
#define FSCACHE_COOKIE_LOOKING_UP	0	
#define FSCACHE_COOKIE_NO_DATA_YET	1	
#define FSCACHE_COOKIE_UNAVAILABLE	2	
#define FSCACHE_COOKIE_INVALIDATING	3	
#define FSCACHE_COOKIE_RELINQUISHED	4	
#define FSCACHE_COOKIE_ENABLED		5	
#define FSCACHE_COOKIE_ENABLEMENT_LOCK	6	
#define FSCACHE_COOKIE_AUX_UPDATED	8	
#define FSCACHE_COOKIE_ACQUIRED		9	
#define FSCACHE_COOKIE_RELINQUISHING	10	

	u8				type;		
	u8				key_len;	
	u8				aux_len;	
	u32				key_hash;	
	union {
		void			*key;		
		u8			inline_key[16];	
	};
	union {
		void			*aux;		
		u8			inline_aux[8];	
	};
};

static inline bool fscache_cookie_enabled(struct fscache_cookie *cookie)
{
	return test_bit(FSCACHE_COOKIE_ENABLED, &cookie->flags);
}


extern int __fscache_register_netfs(struct fscache_netfs *);
extern void __fscache_unregister_netfs(struct fscache_netfs *);
extern struct fscache_cache_tag *__fscache_lookup_cache_tag(const char *);
extern void __fscache_release_cache_tag(struct fscache_cache_tag *);

extern struct fscache_cookie *__fscache_acquire_cookie(
	struct fscache_cookie *,
	const struct fscache_cookie_def *,
	const void *, size_t,
	const void *, size_t,
	void *, loff_t, bool);
extern void __fscache_relinquish_cookie(struct fscache_cookie *, const void *, bool);
extern int __fscache_check_consistency(struct fscache_cookie *, const void *);
extern void __fscache_update_cookie(struct fscache_cookie *, const void *);
extern int __fscache_attr_changed(struct fscache_cookie *);
extern void __fscache_invalidate(struct fscache_cookie *);
extern void __fscache_wait_on_invalidate(struct fscache_cookie *);
extern int __fscache_read_or_alloc_page(struct fscache_cookie *,
					struct page *,
					fscache_rw_complete_t,
					void *,
					gfp_t);
extern int __fscache_read_or_alloc_pages(struct fscache_cookie *,
					 struct address_space *,
					 struct list_head *,
					 unsigned *,
					 fscache_rw_complete_t,
					 void *,
					 gfp_t);
extern int __fscache_alloc_page(struct fscache_cookie *, struct page *, gfp_t);
extern int __fscache_write_page(struct fscache_cookie *, struct page *, loff_t, gfp_t);
extern void __fscache_uncache_page(struct fscache_cookie *, struct page *);
extern bool __fscache_check_page_write(struct fscache_cookie *, struct page *);
extern void __fscache_wait_on_page_write(struct fscache_cookie *, struct page *);
extern bool __fscache_maybe_release_page(struct fscache_cookie *, struct page *,
					 gfp_t);
extern void __fscache_uncache_all_inode_pages(struct fscache_cookie *,
					      struct inode *);
extern void __fscache_readpages_cancel(struct fscache_cookie *cookie,
				       struct list_head *pages);
extern void __fscache_disable_cookie(struct fscache_cookie *, const void *, bool);
extern void __fscache_enable_cookie(struct fscache_cookie *, const void *, loff_t,
				    bool (*)(void *), void *);


static inline
int fscache_register_netfs(struct fscache_netfs *netfs)
{
	if (fscache_available())
		return __fscache_register_netfs(netfs);
	else
		return 0;
}


static inline
void fscache_unregister_netfs(struct fscache_netfs *netfs)
{
	if (fscache_available())
		__fscache_unregister_netfs(netfs);
}


static inline
struct fscache_cache_tag *fscache_lookup_cache_tag(const char *name)
{
	if (fscache_available())
		return __fscache_lookup_cache_tag(name);
	else
		return NULL;
}


static inline
void fscache_release_cache_tag(struct fscache_cache_tag *tag)
{
	if (fscache_available())
		__fscache_release_cache_tag(tag);
}


static inline
struct fscache_cookie *fscache_acquire_cookie(
	struct fscache_cookie *parent,
	const struct fscache_cookie_def *def,
	const void *index_key,
	size_t index_key_len,
	const void *aux_data,
	size_t aux_data_len,
	void *netfs_data,
	loff_t object_size,
	bool enable)
{
	if (fscache_cookie_valid(parent) && fscache_cookie_enabled(parent))
		return __fscache_acquire_cookie(parent, def,
						index_key, index_key_len,
						aux_data, aux_data_len,
						netfs_data, object_size, enable);
	else
		return NULL;
}


static inline
void fscache_relinquish_cookie(struct fscache_cookie *cookie,
			       const void *aux_data,
			       bool retire)
{
	if (fscache_cookie_valid(cookie))
		__fscache_relinquish_cookie(cookie, aux_data, retire);
}


static inline
int fscache_check_consistency(struct fscache_cookie *cookie,
			      const void *aux_data)
{
	if (fscache_cookie_valid(cookie) && fscache_cookie_enabled(cookie))
		return __fscache_check_consistency(cookie, aux_data);
	else
		return 0;
}


static inline
void fscache_update_cookie(struct fscache_cookie *cookie, const void *aux_data)
{
	if (fscache_cookie_valid(cookie) && fscache_cookie_enabled(cookie))
		__fscache_update_cookie(cookie, aux_data);
}


static inline
int fscache_pin_cookie(struct fscache_cookie *cookie)
{
	return -ENOBUFS;
}


static inline
void fscache_unpin_cookie(struct fscache_cookie *cookie)
{
}


static inline
int fscache_attr_changed(struct fscache_cookie *cookie)
{
	if (fscache_cookie_valid(cookie) && fscache_cookie_enabled(cookie))
		return __fscache_attr_changed(cookie);
	else
		return -ENOBUFS;
}


static inline
void fscache_invalidate(struct fscache_cookie *cookie)
{
	if (fscache_cookie_valid(cookie) && fscache_cookie_enabled(cookie))
		__fscache_invalidate(cookie);
}


static inline
void fscache_wait_on_invalidate(struct fscache_cookie *cookie)
{
	if (fscache_cookie_valid(cookie))
		__fscache_wait_on_invalidate(cookie);
}


static inline
int fscache_reserve_space(struct fscache_cookie *cookie, loff_t size)
{
	return -ENOBUFS;
}


static inline
int fscache_read_or_alloc_page(struct fscache_cookie *cookie,
			       struct page *page,
			       fscache_rw_complete_t end_io_func,
			       void *context,
			       gfp_t gfp)
{
	if (fscache_cookie_valid(cookie) && fscache_cookie_enabled(cookie))
		return __fscache_read_or_alloc_page(cookie, page, end_io_func,
						    context, gfp);
	else
		return -ENOBUFS;
}


static inline
int fscache_read_or_alloc_pages(struct fscache_cookie *cookie,
				struct address_space *mapping,
				struct list_head *pages,
				unsigned *nr_pages,
				fscache_rw_complete_t end_io_func,
				void *context,
				gfp_t gfp)
{
	if (fscache_cookie_valid(cookie) && fscache_cookie_enabled(cookie))
		return __fscache_read_or_alloc_pages(cookie, mapping, pages,
						     nr_pages, end_io_func,
						     context, gfp);
	else
		return -ENOBUFS;
}


static inline
int fscache_alloc_page(struct fscache_cookie *cookie,
		       struct page *page,
		       gfp_t gfp)
{
	if (fscache_cookie_valid(cookie) && fscache_cookie_enabled(cookie))
		return __fscache_alloc_page(cookie, page, gfp);
	else
		return -ENOBUFS;
}


static inline
void fscache_readpages_cancel(struct fscache_cookie *cookie,
			      struct list_head *pages)
{
	if (fscache_cookie_valid(cookie))
		__fscache_readpages_cancel(cookie, pages);
}


static inline
int fscache_write_page(struct fscache_cookie *cookie,
		       struct page *page,
		       loff_t object_size,
		       gfp_t gfp)
{
	if (fscache_cookie_valid(cookie) && fscache_cookie_enabled(cookie))
		return __fscache_write_page(cookie, page, object_size, gfp);
	else
		return -ENOBUFS;
}


static inline
void fscache_uncache_page(struct fscache_cookie *cookie,
			  struct page *page)
{
	if (fscache_cookie_valid(cookie))
		__fscache_uncache_page(cookie, page);
}


static inline
bool fscache_check_page_write(struct fscache_cookie *cookie,
			      struct page *page)
{
	if (fscache_cookie_valid(cookie))
		return __fscache_check_page_write(cookie, page);
	return false;
}


static inline
void fscache_wait_on_page_write(struct fscache_cookie *cookie,
				struct page *page)
{
	if (fscache_cookie_valid(cookie))
		__fscache_wait_on_page_write(cookie, page);
}


static inline
bool fscache_maybe_release_page(struct fscache_cookie *cookie,
				struct page *page,
				gfp_t gfp)
{
	if (fscache_cookie_valid(cookie) && PageFsCache(page))
		return __fscache_maybe_release_page(cookie, page, gfp);
	return true;
}


static inline
void fscache_uncache_all_inode_pages(struct fscache_cookie *cookie,
				     struct inode *inode)
{
	if (fscache_cookie_valid(cookie))
		__fscache_uncache_all_inode_pages(cookie, inode);
}


static inline
void fscache_disable_cookie(struct fscache_cookie *cookie,
			    const void *aux_data,
			    bool invalidate)
{
	if (fscache_cookie_valid(cookie) && fscache_cookie_enabled(cookie))
		__fscache_disable_cookie(cookie, aux_data, invalidate);
}


static inline
void fscache_enable_cookie(struct fscache_cookie *cookie,
			   const void *aux_data,
			   loff_t object_size,
			   bool (*can_enable)(void *data),
			   void *data)
{
	if (fscache_cookie_valid(cookie) && !fscache_cookie_enabled(cookie))
		__fscache_enable_cookie(cookie, aux_data, object_size,
					can_enable, data);
}

#endif 
