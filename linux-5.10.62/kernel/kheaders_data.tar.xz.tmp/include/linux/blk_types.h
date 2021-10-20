/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __LINUX_BLK_TYPES_H
#define __LINUX_BLK_TYPES_H

#include <linux/types.h>
#include <linux/bvec.h>
#include <linux/ktime.h>

struct bio_set;
struct bio;
struct bio_integrity_payload;
struct page;
struct io_context;
struct cgroup_subsys_state;
typedef void (bio_end_io_t) (struct bio *);
struct bio_crypt_ctx;

struct block_device {
	dev_t			bd_dev;
	int			bd_openers;
	struct inode *		bd_inode;	
	struct super_block *	bd_super;
	struct mutex		bd_mutex;	
	void *			bd_claiming;
	void *			bd_holder;
	int			bd_holders;
	bool			bd_write_holder;
#ifdef CONFIG_SYSFS
	struct list_head	bd_holder_disks;
#endif
	struct block_device *	bd_contains;
	u8			bd_partno;
	struct hd_struct *	bd_part;
	
	unsigned		bd_part_count;

	spinlock_t		bd_size_lock; 
	struct gendisk *	bd_disk;
	struct backing_dev_info *bd_bdi;

	
	int			bd_fsfreeze_count;
	
	struct mutex		bd_fsfreeze_mutex;
} __randomize_layout;


#if defined(CONFIG_ALPHA) && !defined(__alpha_bwx__)
typedef u32 __bitwise blk_status_t;
#else
typedef u8 __bitwise blk_status_t;
#endif
#define	BLK_STS_OK 0
#define BLK_STS_NOTSUPP		((__force blk_status_t)1)
#define BLK_STS_TIMEOUT		((__force blk_status_t)2)
#define BLK_STS_NOSPC		((__force blk_status_t)3)
#define BLK_STS_TRANSPORT	((__force blk_status_t)4)
#define BLK_STS_TARGET		((__force blk_status_t)5)
#define BLK_STS_NEXUS		((__force blk_status_t)6)
#define BLK_STS_MEDIUM		((__force blk_status_t)7)
#define BLK_STS_PROTECTION	((__force blk_status_t)8)
#define BLK_STS_RESOURCE	((__force blk_status_t)9)
#define BLK_STS_IOERR		((__force blk_status_t)10)


#define BLK_STS_DM_REQUEUE    ((__force blk_status_t)11)

#define BLK_STS_AGAIN		((__force blk_status_t)12)


#define BLK_STS_DEV_RESOURCE	((__force blk_status_t)13)


#define BLK_STS_ZONE_RESOURCE	((__force blk_status_t)14)


#define BLK_STS_ZONE_OPEN_RESOURCE	((__force blk_status_t)15)


#define BLK_STS_ZONE_ACTIVE_RESOURCE	((__force blk_status_t)16)


static inline bool blk_path_error(blk_status_t error)
{
	switch (error) {
	case BLK_STS_NOTSUPP:
	case BLK_STS_NOSPC:
	case BLK_STS_TARGET:
	case BLK_STS_NEXUS:
	case BLK_STS_MEDIUM:
	case BLK_STS_PROTECTION:
		return false;
	}

	
	return true;
}


#define BIO_ISSUE_RES_BITS      1
#define BIO_ISSUE_SIZE_BITS     12
#define BIO_ISSUE_RES_SHIFT     (64 - BIO_ISSUE_RES_BITS)
#define BIO_ISSUE_SIZE_SHIFT    (BIO_ISSUE_RES_SHIFT - BIO_ISSUE_SIZE_BITS)
#define BIO_ISSUE_TIME_MASK     ((1ULL << BIO_ISSUE_SIZE_SHIFT) - 1)
#define BIO_ISSUE_SIZE_MASK     \
	(((1ULL << BIO_ISSUE_SIZE_BITS) - 1) << BIO_ISSUE_SIZE_SHIFT)
#define BIO_ISSUE_RES_MASK      (~((1ULL << BIO_ISSUE_RES_SHIFT) - 1))


#define BIO_ISSUE_THROTL_SKIP_LATENCY (1ULL << 63)

struct bio_issue {
	u64 value;
};

static inline u64 __bio_issue_time(u64 time)
{
	return time & BIO_ISSUE_TIME_MASK;
}

static inline u64 bio_issue_time(struct bio_issue *issue)
{
	return __bio_issue_time(issue->value);
}

static inline sector_t bio_issue_size(struct bio_issue *issue)
{
	return ((issue->value & BIO_ISSUE_SIZE_MASK) >> BIO_ISSUE_SIZE_SHIFT);
}

static inline void bio_issue_init(struct bio_issue *issue,
				       sector_t size)
{
	size &= (1ULL << BIO_ISSUE_SIZE_BITS) - 1;
	issue->value = ((issue->value & BIO_ISSUE_RES_MASK) |
			(ktime_get_ns() & BIO_ISSUE_TIME_MASK) |
			((u64)size << BIO_ISSUE_SIZE_SHIFT));
}


struct bio {
	struct bio		*bi_next;	
	struct gendisk		*bi_disk;
	unsigned int		bi_opf;		
	unsigned short		bi_flags;	
	unsigned short		bi_ioprio;
	unsigned short		bi_write_hint;
	blk_status_t		bi_status;
	u8			bi_partno;
	atomic_t		__bi_remaining;

	struct bvec_iter	bi_iter;

	bio_end_io_t		*bi_end_io;

	void			*bi_private;
#ifdef CONFIG_BLK_CGROUP
	
	struct blkcg_gq		*bi_blkg;
	struct bio_issue	bi_issue;
#ifdef CONFIG_BLK_CGROUP_IOCOST
	u64			bi_iocost_cost;
#endif
#endif

#ifdef CONFIG_BLK_INLINE_ENCRYPTION
	struct bio_crypt_ctx	*bi_crypt_context;
#endif

	union {
#if defined(CONFIG_BLK_DEV_INTEGRITY)
		struct bio_integrity_payload *bi_integrity; 
#endif
	};

	unsigned short		bi_vcnt;	

	

	unsigned short		bi_max_vecs;	

	atomic_t		__bi_cnt;	

	struct bio_vec		*bi_io_vec;	

	struct bio_set		*bi_pool;

	
	struct bio_vec		bi_inline_vecs[];
};

#define BIO_RESET_BYTES		offsetof(struct bio, bi_max_vecs)


enum {
	BIO_NO_PAGE_REF,	
	BIO_CLONED,		
	BIO_BOUNCED,		
	BIO_WORKINGSET,		
	BIO_QUIET,		
	BIO_CHAIN,		
	BIO_REFFED,		
	BIO_THROTTLED,		
	BIO_TRACE_COMPLETION,	
	BIO_CGROUP_ACCT,	
	BIO_TRACKED,		
	BIO_FLAG_LAST
};




#define BVEC_POOL_NR		6
#define BVEC_POOL_MAX		(BVEC_POOL_NR - 1)


#define BVEC_POOL_BITS		(3)
#define BVEC_POOL_OFFSET	(16 - BVEC_POOL_BITS)
#define BVEC_POOL_IDX(bio)	((bio)->bi_flags >> BVEC_POOL_OFFSET)
#if (1<< BVEC_POOL_BITS) < (BVEC_POOL_NR+1)
# error "BVEC_POOL_BITS is too small"
#endif


#define BIO_RESET_BITS	BVEC_POOL_OFFSET

typedef __u32 __bitwise blk_mq_req_flags_t;


#define REQ_OP_BITS	8
#define REQ_OP_MASK	((1 << REQ_OP_BITS) - 1)
#define REQ_FLAG_BITS	24

enum req_opf {
	
	REQ_OP_READ		= 0,
	
	REQ_OP_WRITE		= 1,
	
	REQ_OP_FLUSH		= 2,
	
	REQ_OP_DISCARD		= 3,
	
	REQ_OP_SECURE_ERASE	= 5,
	
	REQ_OP_WRITE_SAME	= 7,
	
	REQ_OP_WRITE_ZEROES	= 9,
	
	REQ_OP_ZONE_OPEN	= 10,
	
	REQ_OP_ZONE_CLOSE	= 11,
	
	REQ_OP_ZONE_FINISH	= 12,
	
	REQ_OP_ZONE_APPEND	= 13,
	
	REQ_OP_ZONE_RESET	= 15,
	
	REQ_OP_ZONE_RESET_ALL	= 17,

	
	REQ_OP_SCSI_IN		= 32,
	REQ_OP_SCSI_OUT		= 33,
	
	REQ_OP_DRV_IN		= 34,
	REQ_OP_DRV_OUT		= 35,

	REQ_OP_LAST,
};

enum req_flag_bits {
	__REQ_FAILFAST_DEV =	
		REQ_OP_BITS,
	__REQ_FAILFAST_TRANSPORT, 
	__REQ_FAILFAST_DRIVER,	
	__REQ_SYNC,		
	__REQ_META,		
	__REQ_PRIO,		
	__REQ_NOMERGE,		
	__REQ_IDLE,		
	__REQ_INTEGRITY,	
	__REQ_FUA,		
	__REQ_PREFLUSH,		
	__REQ_RAHEAD,		
	__REQ_BACKGROUND,	
	__REQ_NOWAIT,           
	
	__REQ_CGROUP_PUNT,

	
	__REQ_NOUNMAP,		

	__REQ_HIPRI,

	
	__REQ_DRV,
	__REQ_SWAP,		
	__REQ_NR_BITS,		
};

#define REQ_FAILFAST_DEV	(1ULL << __REQ_FAILFAST_DEV)
#define REQ_FAILFAST_TRANSPORT	(1ULL << __REQ_FAILFAST_TRANSPORT)
#define REQ_FAILFAST_DRIVER	(1ULL << __REQ_FAILFAST_DRIVER)
#define REQ_SYNC		(1ULL << __REQ_SYNC)
#define REQ_META		(1ULL << __REQ_META)
#define REQ_PRIO		(1ULL << __REQ_PRIO)
#define REQ_NOMERGE		(1ULL << __REQ_NOMERGE)
#define REQ_IDLE		(1ULL << __REQ_IDLE)
#define REQ_INTEGRITY		(1ULL << __REQ_INTEGRITY)
#define REQ_FUA			(1ULL << __REQ_FUA)
#define REQ_PREFLUSH		(1ULL << __REQ_PREFLUSH)
#define REQ_RAHEAD		(1ULL << __REQ_RAHEAD)
#define REQ_BACKGROUND		(1ULL << __REQ_BACKGROUND)
#define REQ_NOWAIT		(1ULL << __REQ_NOWAIT)
#define REQ_CGROUP_PUNT		(1ULL << __REQ_CGROUP_PUNT)

#define REQ_NOUNMAP		(1ULL << __REQ_NOUNMAP)
#define REQ_HIPRI		(1ULL << __REQ_HIPRI)

#define REQ_DRV			(1ULL << __REQ_DRV)
#define REQ_SWAP		(1ULL << __REQ_SWAP)

#define REQ_FAILFAST_MASK \
	(REQ_FAILFAST_DEV | REQ_FAILFAST_TRANSPORT | REQ_FAILFAST_DRIVER)

#define REQ_NOMERGE_FLAGS \
	(REQ_NOMERGE | REQ_PREFLUSH | REQ_FUA)

enum stat_group {
	STAT_READ,
	STAT_WRITE,
	STAT_DISCARD,
	STAT_FLUSH,

	NR_STAT_GROUPS
};

#define bio_op(bio) \
	((bio)->bi_opf & REQ_OP_MASK)
#define req_op(req) \
	((req)->cmd_flags & REQ_OP_MASK)


static inline void bio_set_op_attrs(struct bio *bio, unsigned op,
		unsigned op_flags)
{
	bio->bi_opf = op | op_flags;
}

static inline bool op_is_write(unsigned int op)
{
	return (op & 1);
}


static inline bool op_is_flush(unsigned int op)
{
	return op & (REQ_FUA | REQ_PREFLUSH);
}


static inline bool op_is_sync(unsigned int op)
{
	return (op & REQ_OP_MASK) == REQ_OP_READ ||
		(op & (REQ_SYNC | REQ_FUA | REQ_PREFLUSH));
}

static inline bool op_is_discard(unsigned int op)
{
	return (op & REQ_OP_MASK) == REQ_OP_DISCARD;
}


static inline bool op_is_zone_mgmt(enum req_opf op)
{
	switch (op & REQ_OP_MASK) {
	case REQ_OP_ZONE_RESET:
	case REQ_OP_ZONE_OPEN:
	case REQ_OP_ZONE_CLOSE:
	case REQ_OP_ZONE_FINISH:
		return true;
	default:
		return false;
	}
}

static inline int op_stat_group(unsigned int op)
{
	if (op_is_discard(op))
		return STAT_DISCARD;
	return op_is_write(op);
}

typedef unsigned int blk_qc_t;
#define BLK_QC_T_NONE		-1U
#define BLK_QC_T_SHIFT		16
#define BLK_QC_T_INTERNAL	(1U << 31)

static inline bool blk_qc_t_valid(blk_qc_t cookie)
{
	return cookie != BLK_QC_T_NONE;
}

static inline unsigned int blk_qc_t_to_queue_num(blk_qc_t cookie)
{
	return (cookie & ~BLK_QC_T_INTERNAL) >> BLK_QC_T_SHIFT;
}

static inline unsigned int blk_qc_t_to_tag(blk_qc_t cookie)
{
	return cookie & ((1u << BLK_QC_T_SHIFT) - 1);
}

static inline bool blk_qc_t_is_internal(blk_qc_t cookie)
{
	return (cookie & BLK_QC_T_INTERNAL) != 0;
}

struct blk_rq_stat {
	u64 mean;
	u64 min;
	u64 max;
	u32 nr_samples;
	u64 batch;
};

#endif 
