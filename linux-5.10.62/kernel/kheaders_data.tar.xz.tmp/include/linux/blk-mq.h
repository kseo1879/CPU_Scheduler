/* SPDX-License-Identifier: GPL-2.0 */
#ifndef BLK_MQ_H
#define BLK_MQ_H

#include <linux/blkdev.h>
#include <linux/sbitmap.h>
#include <linux/srcu.h>

struct blk_mq_tags;
struct blk_flush_queue;


struct blk_mq_hw_ctx {
	struct {
		
		spinlock_t		lock;
		
		struct list_head	dispatch;
		 
		unsigned long		state;
	} ____cacheline_aligned_in_smp;

	
	struct delayed_work	run_work;
	
	cpumask_var_t		cpumask;
	
	int			next_cpu;
	
	int			next_cpu_batch;

	
	unsigned long		flags;

	
	void			*sched_data;
	
	struct request_queue	*queue;
	
	struct blk_flush_queue	*fq;

	
	void			*driver_data;

	
	struct sbitmap		ctx_map;

	
	struct blk_mq_ctx	*dispatch_from;
	
	unsigned int		dispatch_busy;

	
	unsigned short		type;
	
	unsigned short		nr_ctx;
	
	struct blk_mq_ctx	**ctxs;

	
	spinlock_t		dispatch_wait_lock;
	
	wait_queue_entry_t	dispatch_wait;

	
	atomic_t		wait_index;

	
	struct blk_mq_tags	*tags;
	
	struct blk_mq_tags	*sched_tags;

	
	unsigned long		queued;
	
	unsigned long		run;
#define BLK_MQ_MAX_DISPATCH_ORDER	7
	
	unsigned long		dispatched[BLK_MQ_MAX_DISPATCH_ORDER];

	
	unsigned int		numa_node;
	
	unsigned int		queue_num;

	
	atomic_t		nr_active;
	
	atomic_t                elevator_queued;

	
	struct hlist_node	cpuhp_online;
	
	struct hlist_node	cpuhp_dead;
	
	struct kobject		kobj;

	
	unsigned long		poll_considered;
	
	unsigned long		poll_invoked;
	
	unsigned long		poll_success;

#ifdef CONFIG_BLK_DEBUG_FS
	
	struct dentry		*debugfs_dir;
	
	struct dentry		*sched_debugfs_dir;
#endif

	
	struct list_head	hctx_list;

	
	struct srcu_struct	srcu[];
};


struct blk_mq_queue_map {
	unsigned int *mq_map;
	unsigned int nr_queues;
	unsigned int queue_offset;
};


enum hctx_type {
	HCTX_TYPE_DEFAULT,
	HCTX_TYPE_READ,
	HCTX_TYPE_POLL,

	HCTX_MAX_TYPES,
};


struct blk_mq_tag_set {
	struct blk_mq_queue_map	map[HCTX_MAX_TYPES];
	unsigned int		nr_maps;
	const struct blk_mq_ops	*ops;
	unsigned int		nr_hw_queues;
	unsigned int		queue_depth;
	unsigned int		reserved_tags;
	unsigned int		cmd_size;
	int			numa_node;
	unsigned int		timeout;
	unsigned int		flags;
	void			*driver_data;
	atomic_t		active_queues_shared_sbitmap;

	struct sbitmap_queue	__bitmap_tags;
	struct sbitmap_queue	__breserved_tags;
	struct blk_mq_tags	**tags;

	struct mutex		tag_list_lock;
	struct list_head	tag_list;
};


struct blk_mq_queue_data {
	struct request *rq;
	bool last;
};

typedef bool (busy_iter_fn)(struct blk_mq_hw_ctx *, struct request *, void *,
		bool);
typedef bool (busy_tag_iter_fn)(struct request *, void *, bool);


struct blk_mq_ops {
	
	blk_status_t (*queue_rq)(struct blk_mq_hw_ctx *,
				 const struct blk_mq_queue_data *);

	
	void (*commit_rqs)(struct blk_mq_hw_ctx *);

	
	bool (*get_budget)(struct request_queue *);

	
	void (*put_budget)(struct request_queue *);

	
	enum blk_eh_timer_return (*timeout)(struct request *, bool);

	
	int (*poll)(struct blk_mq_hw_ctx *);

	
	void (*complete)(struct request *);

	
	int (*init_hctx)(struct blk_mq_hw_ctx *, void *, unsigned int);
	
	void (*exit_hctx)(struct blk_mq_hw_ctx *, unsigned int);

	
	int (*init_request)(struct blk_mq_tag_set *set, struct request *,
			    unsigned int, unsigned int);
	
	void (*exit_request)(struct blk_mq_tag_set *set, struct request *,
			     unsigned int);

	
	void (*initialize_rq_fn)(struct request *rq);

	
	void (*cleanup_rq)(struct request *);

	
	bool (*busy)(struct request_queue *);

	
	int (*map_queues)(struct blk_mq_tag_set *set);

#ifdef CONFIG_BLK_DEBUG_FS
	
	void (*show_rq)(struct seq_file *m, struct request *rq);
#endif
};

enum {
	BLK_MQ_F_SHOULD_MERGE	= 1 << 0,
	BLK_MQ_F_TAG_QUEUE_SHARED = 1 << 1,
	
	BLK_MQ_F_STACKING	= 1 << 2,
	BLK_MQ_F_TAG_HCTX_SHARED = 1 << 3,
	BLK_MQ_F_BLOCKING	= 1 << 5,
	BLK_MQ_F_NO_SCHED	= 1 << 6,
	BLK_MQ_F_ALLOC_POLICY_START_BIT = 8,
	BLK_MQ_F_ALLOC_POLICY_BITS = 1,

	BLK_MQ_S_STOPPED	= 0,
	BLK_MQ_S_TAG_ACTIVE	= 1,
	BLK_MQ_S_SCHED_RESTART	= 2,

	
	BLK_MQ_S_INACTIVE	= 3,

	BLK_MQ_MAX_DEPTH	= 10240,

	BLK_MQ_CPU_WORK_BATCH	= 8,
};
#define BLK_MQ_FLAG_TO_ALLOC_POLICY(flags) \
	((flags >> BLK_MQ_F_ALLOC_POLICY_START_BIT) & \
		((1 << BLK_MQ_F_ALLOC_POLICY_BITS) - 1))
#define BLK_ALLOC_POLICY_TO_MQ_FLAG(policy) \
	((policy & ((1 << BLK_MQ_F_ALLOC_POLICY_BITS) - 1)) \
		<< BLK_MQ_F_ALLOC_POLICY_START_BIT)

struct request_queue *blk_mq_init_queue(struct blk_mq_tag_set *);
struct request_queue *blk_mq_init_queue_data(struct blk_mq_tag_set *set,
		void *queuedata);
struct request_queue *blk_mq_init_allocated_queue(struct blk_mq_tag_set *set,
						  struct request_queue *q,
						  bool elevator_init);
struct request_queue *blk_mq_init_sq_queue(struct blk_mq_tag_set *set,
						const struct blk_mq_ops *ops,
						unsigned int queue_depth,
						unsigned int set_flags);
void blk_mq_unregister_dev(struct device *, struct request_queue *);

int blk_mq_alloc_tag_set(struct blk_mq_tag_set *set);
void blk_mq_free_tag_set(struct blk_mq_tag_set *set);

void blk_mq_flush_plug_list(struct blk_plug *plug, bool from_schedule);

void blk_mq_free_request(struct request *rq);

bool blk_mq_queue_inflight(struct request_queue *q);

enum {
	
	BLK_MQ_REQ_NOWAIT	= (__force blk_mq_req_flags_t)(1 << 0),
	
	BLK_MQ_REQ_RESERVED	= (__force blk_mq_req_flags_t)(1 << 1),
	
	BLK_MQ_REQ_PM		= (__force blk_mq_req_flags_t)(1 << 2),
};

struct request *blk_mq_alloc_request(struct request_queue *q, unsigned int op,
		blk_mq_req_flags_t flags);
struct request *blk_mq_alloc_request_hctx(struct request_queue *q,
		unsigned int op, blk_mq_req_flags_t flags,
		unsigned int hctx_idx);
struct request *blk_mq_tag_to_rq(struct blk_mq_tags *tags, unsigned int tag);

enum {
	BLK_MQ_UNIQUE_TAG_BITS = 16,
	BLK_MQ_UNIQUE_TAG_MASK = (1 << BLK_MQ_UNIQUE_TAG_BITS) - 1,
};

u32 blk_mq_unique_tag(struct request *rq);

static inline u16 blk_mq_unique_tag_to_hwq(u32 unique_tag)
{
	return unique_tag >> BLK_MQ_UNIQUE_TAG_BITS;
}

static inline u16 blk_mq_unique_tag_to_tag(u32 unique_tag)
{
	return unique_tag & BLK_MQ_UNIQUE_TAG_MASK;
}


static inline enum mq_rq_state blk_mq_rq_state(struct request *rq)
{
	return READ_ONCE(rq->state);
}

static inline int blk_mq_request_started(struct request *rq)
{
	return blk_mq_rq_state(rq) != MQ_RQ_IDLE;
}

static inline int blk_mq_request_completed(struct request *rq)
{
	return blk_mq_rq_state(rq) == MQ_RQ_COMPLETE;
}

void blk_mq_start_request(struct request *rq);
void blk_mq_end_request(struct request *rq, blk_status_t error);
void __blk_mq_end_request(struct request *rq, blk_status_t error);

void blk_mq_requeue_request(struct request *rq, bool kick_requeue_list);
void blk_mq_kick_requeue_list(struct request_queue *q);
void blk_mq_delay_kick_requeue_list(struct request_queue *q, unsigned long msecs);
void blk_mq_complete_request(struct request *rq);
bool blk_mq_complete_request_remote(struct request *rq);
bool blk_mq_queue_stopped(struct request_queue *q);
void blk_mq_stop_hw_queue(struct blk_mq_hw_ctx *hctx);
void blk_mq_start_hw_queue(struct blk_mq_hw_ctx *hctx);
void blk_mq_stop_hw_queues(struct request_queue *q);
void blk_mq_start_hw_queues(struct request_queue *q);
void blk_mq_start_stopped_hw_queue(struct blk_mq_hw_ctx *hctx, bool async);
void blk_mq_start_stopped_hw_queues(struct request_queue *q, bool async);
void blk_mq_quiesce_queue(struct request_queue *q);
void blk_mq_unquiesce_queue(struct request_queue *q);
void blk_mq_delay_run_hw_queue(struct blk_mq_hw_ctx *hctx, unsigned long msecs);
void blk_mq_run_hw_queue(struct blk_mq_hw_ctx *hctx, bool async);
void blk_mq_run_hw_queues(struct request_queue *q, bool async);
void blk_mq_delay_run_hw_queues(struct request_queue *q, unsigned long msecs);
void blk_mq_tagset_busy_iter(struct blk_mq_tag_set *tagset,
		busy_tag_iter_fn *fn, void *priv);
void blk_mq_tagset_wait_completed_request(struct blk_mq_tag_set *tagset);
void blk_mq_freeze_queue(struct request_queue *q);
void blk_mq_unfreeze_queue(struct request_queue *q);
void blk_freeze_queue_start(struct request_queue *q);
void blk_mq_freeze_queue_wait(struct request_queue *q);
int blk_mq_freeze_queue_wait_timeout(struct request_queue *q,
				     unsigned long timeout);

int blk_mq_map_queues(struct blk_mq_queue_map *qmap);
void blk_mq_update_nr_hw_queues(struct blk_mq_tag_set *set, int nr_hw_queues);

void blk_mq_quiesce_queue_nowait(struct request_queue *q);

unsigned int blk_mq_rq_cpu(struct request *rq);

bool __blk_should_fake_timeout(struct request_queue *q);
static inline bool blk_should_fake_timeout(struct request_queue *q)
{
	if (IS_ENABLED(CONFIG_FAIL_IO_TIMEOUT) &&
	    test_bit(QUEUE_FLAG_FAIL_IO, &q->queue_flags))
		return __blk_should_fake_timeout(q);
	return false;
}


static inline struct request *blk_mq_rq_from_pdu(void *pdu)
{
	return pdu - sizeof(struct request);
}


static inline void *blk_mq_rq_to_pdu(struct request *rq)
{
	return rq + 1;
}

#define queue_for_each_hw_ctx(q, hctx, i)				\
	for ((i) = 0; (i) < (q)->nr_hw_queues &&			\
	     ({ hctx = (q)->queue_hw_ctx[i]; 1; }); (i)++)

#define hctx_for_each_ctx(hctx, ctx, i)					\
	for ((i) = 0; (i) < (hctx)->nr_ctx &&				\
	     ({ ctx = (hctx)->ctxs[(i)]; 1; }); (i)++)

static inline blk_qc_t request_to_qc_t(struct blk_mq_hw_ctx *hctx,
		struct request *rq)
{
	if (rq->tag != -1)
		return rq->tag | (hctx->queue_num << BLK_QC_T_SHIFT);

	return rq->internal_tag | (hctx->queue_num << BLK_QC_T_SHIFT) |
			BLK_QC_T_INTERNAL;
}

static inline void blk_mq_cleanup_rq(struct request *rq)
{
	if (rq->q->mq_ops->cleanup_rq)
		rq->q->mq_ops->cleanup_rq(rq);
}

blk_qc_t blk_mq_submit_bio(struct bio *bio);

#endif
