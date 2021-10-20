/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_GENHD_H
#define _LINUX_GENHD_H



#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/rcupdate.h>
#include <linux/slab.h>
#include <linux/percpu-refcount.h>
#include <linux/uuid.h>
#include <linux/blk_types.h>
#include <asm/local.h>

#define dev_to_disk(device)	container_of((device), struct gendisk, part0.__dev)
#define dev_to_part(device)	container_of((device), struct hd_struct, __dev)
#define disk_to_dev(disk)	(&(disk)->part0.__dev)
#define part_to_dev(part)	(&((part)->__dev))

extern const struct device_type disk_type;
extern struct device_type part_type;
extern struct class block_class;

#define DISK_MAX_PARTS			256
#define DISK_NAME_LEN			32

#include <linux/major.h>
#include <linux/device.h>
#include <linux/smp.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/workqueue.h>

#define PARTITION_META_INFO_VOLNAMELTH	64

#define PARTITION_META_INFO_UUIDLTH	(UUID_STRING_LEN + 1)

struct partition_meta_info {
	char uuid[PARTITION_META_INFO_UUIDLTH];
	u8 volname[PARTITION_META_INFO_VOLNAMELTH];
};

struct hd_struct {
	sector_t start_sect;
	
	sector_t nr_sects;
#if BITS_PER_LONG==32 && defined(CONFIG_SMP)
	seqcount_t nr_sects_seq;
#endif
	unsigned long stamp;
	struct disk_stats __percpu *dkstats;
	struct percpu_ref ref;

	struct device __dev;
	struct kobject *holder_dir;
	int policy, partno;
	struct partition_meta_info *info;
#ifdef CONFIG_FAIL_MAKE_REQUEST
	int make_it_fail;
#endif
	struct rcu_work rcu_work;
};


#define GENHD_FL_REMOVABLE			0x0001


#define GENHD_FL_CD				0x0008
#define GENHD_FL_UP				0x0010
#define GENHD_FL_SUPPRESS_PARTITION_INFO	0x0020
#define GENHD_FL_EXT_DEVT			0x0040
#define GENHD_FL_NATIVE_CAPACITY		0x0080
#define GENHD_FL_BLOCK_EVENTS_ON_EXCL_WRITE	0x0100
#define GENHD_FL_NO_PART_SCAN			0x0200
#define GENHD_FL_HIDDEN				0x0400

enum {
	DISK_EVENT_MEDIA_CHANGE			= 1 << 0, 
	DISK_EVENT_EJECT_REQUEST		= 1 << 1, 
};

enum {
	
	DISK_EVENT_FLAG_POLL			= 1 << 0,
	
	DISK_EVENT_FLAG_UEVENT			= 1 << 1,
};

struct disk_part_tbl {
	struct rcu_head rcu_head;
	int len;
	struct hd_struct __rcu *last_lookup;
	struct hd_struct __rcu *part[];
};

struct disk_events;
struct badblocks;

struct blk_integrity {
	const struct blk_integrity_profile	*profile;
	unsigned char				flags;
	unsigned char				tuple_size;
	unsigned char				interval_exp;
	unsigned char				tag_size;
};

struct gendisk {
	
	int major;			
	int first_minor;
	int minors;                     

	char disk_name[DISK_NAME_LEN];	

	unsigned short events;		
	unsigned short event_flags;	

	
	struct disk_part_tbl __rcu *part_tbl;
	struct hd_struct part0;

	const struct block_device_operations *fops;
	struct request_queue *queue;
	void *private_data;

	int flags;
	unsigned long state;
#define GD_NEED_PART_SCAN		0
	struct rw_semaphore lookup_sem;
	struct kobject *slave_dir;

	struct timer_rand_state *random;
	atomic_t sync_io;		
	struct disk_events *ev;
#ifdef  CONFIG_BLK_DEV_INTEGRITY
	struct kobject integrity_kobj;
#endif	
#if IS_ENABLED(CONFIG_CDROM)
	struct cdrom_device_info *cdi;
#endif
	int node_id;
	struct badblocks *bb;
	struct lockdep_map lockdep_map;
};

#if IS_REACHABLE(CONFIG_CDROM)
#define disk_to_cdi(disk)	((disk)->cdi)
#else
#define disk_to_cdi(disk)	NULL
#endif

static inline struct gendisk *part_to_disk(struct hd_struct *part)
{
	if (likely(part)) {
		if (part->partno)
			return dev_to_disk(part_to_dev(part)->parent);
		else
			return dev_to_disk(part_to_dev(part));
	}
	return NULL;
}

static inline int disk_max_parts(struct gendisk *disk)
{
	if (disk->flags & GENHD_FL_EXT_DEVT)
		return DISK_MAX_PARTS;
	return disk->minors;
}

static inline bool disk_part_scan_enabled(struct gendisk *disk)
{
	return disk_max_parts(disk) > 1 &&
		!(disk->flags & GENHD_FL_NO_PART_SCAN);
}

static inline dev_t disk_devt(struct gendisk *disk)
{
	return MKDEV(disk->major, disk->first_minor);
}

static inline dev_t part_devt(struct hd_struct *part)
{
	return part_to_dev(part)->devt;
}

extern struct hd_struct *__disk_get_part(struct gendisk *disk, int partno);
extern struct hd_struct *disk_get_part(struct gendisk *disk, int partno);

static inline void disk_put_part(struct hd_struct *part)
{
	if (likely(part))
		put_device(part_to_dev(part));
}

static inline void hd_sects_seq_init(struct hd_struct *p)
{
#if BITS_PER_LONG==32 && defined(CONFIG_SMP)
	seqcount_init(&p->nr_sects_seq);
#endif
}


#define DISK_PITER_REVERSE	(1 << 0) 
#define DISK_PITER_INCL_EMPTY	(1 << 1) 
#define DISK_PITER_INCL_PART0	(1 << 2) 
#define DISK_PITER_INCL_EMPTY_PART0 (1 << 3) 

struct disk_part_iter {
	struct gendisk		*disk;
	struct hd_struct	*part;
	int			idx;
	unsigned int		flags;
};

extern void disk_part_iter_init(struct disk_part_iter *piter,
				 struct gendisk *disk, unsigned int flags);
extern struct hd_struct *disk_part_iter_next(struct disk_part_iter *piter);
extern void disk_part_iter_exit(struct disk_part_iter *piter);
extern bool disk_has_partitions(struct gendisk *disk);


extern void device_add_disk(struct device *parent, struct gendisk *disk,
			    const struct attribute_group **groups);
static inline void add_disk(struct gendisk *disk)
{
	device_add_disk(NULL, disk, NULL);
}
extern void device_add_disk_no_queue_reg(struct device *parent, struct gendisk *disk);
static inline void add_disk_no_queue_reg(struct gendisk *disk)
{
	device_add_disk_no_queue_reg(NULL, disk);
}

extern void del_gendisk(struct gendisk *gp);
extern struct gendisk *get_gendisk(dev_t dev, int *partno);
extern struct block_device *bdget_disk(struct gendisk *disk, int partno);

extern void set_device_ro(struct block_device *bdev, int flag);
extern void set_disk_ro(struct gendisk *disk, int flag);

static inline int get_disk_ro(struct gendisk *disk)
{
	return disk->part0.policy;
}

extern void disk_block_events(struct gendisk *disk);
extern void disk_unblock_events(struct gendisk *disk);
extern void disk_flush_events(struct gendisk *disk, unsigned int mask);
bool set_capacity_revalidate_and_notify(struct gendisk *disk, sector_t size,
		bool update_bdev);


extern void add_disk_randomness(struct gendisk *disk) __latent_entropy;
extern void rand_initialize_disk(struct gendisk *disk);

static inline sector_t get_start_sect(struct block_device *bdev)
{
	return bdev->bd_part->start_sect;
}
static inline sector_t get_capacity(struct gendisk *disk)
{
	return disk->part0.nr_sects;
}
static inline void set_capacity(struct gendisk *disk, sector_t size)
{
	disk->part0.nr_sects = size;
}

int bdev_disk_changed(struct block_device *bdev, bool invalidate);
int blk_add_partitions(struct gendisk *disk, struct block_device *bdev);
int blk_drop_partitions(struct block_device *bdev);

extern struct gendisk *__alloc_disk_node(int minors, int node_id);
extern struct kobject *get_disk_and_module(struct gendisk *disk);
extern void put_disk(struct gendisk *disk);
extern void put_disk_and_module(struct gendisk *disk);
extern void blk_register_region(dev_t devt, unsigned long range,
			struct module *module,
			struct kobject *(*probe)(dev_t, int *, void *),
			int (*lock)(dev_t, void *),
			void *data);
extern void blk_unregister_region(dev_t devt, unsigned long range);

#define alloc_disk_node(minors, node_id)				\
({									\
	static struct lock_class_key __key;				\
	const char *__name;						\
	struct gendisk *__disk;						\
									\
	__name = "(gendisk_completion)"#minors"("#node_id")";		\
									\
	__disk = __alloc_disk_node(minors, node_id);			\
									\
	if (__disk)							\
		lockdep_init_map(&__disk->lockdep_map, __name, &__key, 0); \
									\
	__disk;								\
})

#define alloc_disk(minors) alloc_disk_node(minors, NUMA_NO_NODE)

int register_blkdev(unsigned int major, const char *name);
void unregister_blkdev(unsigned int major, const char *name);

void revalidate_disk_size(struct gendisk *disk, bool verbose);
bool bdev_check_media_change(struct block_device *bdev);
int __invalidate_device(struct block_device *bdev, bool kill_dirty);
void bd_set_nr_sectors(struct block_device *bdev, sector_t sectors);


int blkdev_ioctl(struct block_device *, fmode_t, unsigned, unsigned long);
long compat_blkdev_ioctl(struct file *, unsigned, unsigned long);

#ifdef CONFIG_SYSFS
int bd_link_disk_holder(struct block_device *bdev, struct gendisk *disk);
void bd_unlink_disk_holder(struct block_device *bdev, struct gendisk *disk);
#else
static inline int bd_link_disk_holder(struct block_device *bdev,
				      struct gendisk *disk)
{
	return 0;
}
static inline void bd_unlink_disk_holder(struct block_device *bdev,
					 struct gendisk *disk)
{
}
#endif 

#ifdef CONFIG_BLOCK
void printk_all_partitions(void);
dev_t blk_lookup_devt(const char *name, int partno);
#else 
static inline void printk_all_partitions(void)
{
}
static inline dev_t blk_lookup_devt(const char *name, int partno)
{
	dev_t devt = MKDEV(0, 0);
	return devt;
}
#endif 

#endif 
