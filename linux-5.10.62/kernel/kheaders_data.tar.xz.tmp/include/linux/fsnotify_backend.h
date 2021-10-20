/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __LINUX_FSNOTIFY_BACKEND_H
#define __LINUX_FSNOTIFY_BACKEND_H

#ifdef __KERNEL__

#include <linux/idr.h> 
#include <linux/fs.h> 
#include <linux/list.h>
#include <linux/path.h> 
#include <linux/spinlock.h>
#include <linux/types.h>
#include <linux/atomic.h>
#include <linux/user_namespace.h>
#include <linux/refcount.h>


#define FS_ACCESS		0x00000001	
#define FS_MODIFY		0x00000002	
#define FS_ATTRIB		0x00000004	
#define FS_CLOSE_WRITE		0x00000008	
#define FS_CLOSE_NOWRITE	0x00000010	
#define FS_OPEN			0x00000020	
#define FS_MOVED_FROM		0x00000040	
#define FS_MOVED_TO		0x00000080	
#define FS_CREATE		0x00000100	
#define FS_DELETE		0x00000200	
#define FS_DELETE_SELF		0x00000400	
#define FS_MOVE_SELF		0x00000800	
#define FS_OPEN_EXEC		0x00001000	

#define FS_UNMOUNT		0x00002000	
#define FS_Q_OVERFLOW		0x00004000	
#define FS_IN_IGNORED		0x00008000	

#define FS_OPEN_PERM		0x00010000	
#define FS_ACCESS_PERM		0x00020000	
#define FS_OPEN_EXEC_PERM	0x00040000	

#define FS_EXCL_UNLINK		0x04000000	

#define FS_EVENT_ON_CHILD	0x08000000

#define FS_DN_RENAME		0x10000000	
#define FS_DN_MULTISHOT		0x20000000	
#define FS_ISDIR		0x40000000	
#define FS_IN_ONESHOT		0x80000000	

#define FS_MOVE			(FS_MOVED_FROM | FS_MOVED_TO)


#define ALL_FSNOTIFY_DIRENT_EVENTS	(FS_CREATE | FS_DELETE | FS_MOVE)

#define ALL_FSNOTIFY_PERM_EVENTS (FS_OPEN_PERM | FS_ACCESS_PERM | \
				  FS_OPEN_EXEC_PERM)


#define FS_EVENTS_POSS_ON_CHILD   (ALL_FSNOTIFY_PERM_EVENTS | \
				   FS_ACCESS | FS_MODIFY | FS_ATTRIB | \
				   FS_CLOSE_WRITE | FS_CLOSE_NOWRITE | \
				   FS_OPEN | FS_OPEN_EXEC)


#define FS_EVENTS_POSS_TO_PARENT (FS_EVENTS_POSS_ON_CHILD)


#define ALL_FSNOTIFY_EVENTS (ALL_FSNOTIFY_DIRENT_EVENTS | \
			     FS_EVENTS_POSS_ON_CHILD | \
			     FS_DELETE_SELF | FS_MOVE_SELF | FS_DN_RENAME | \
			     FS_UNMOUNT | FS_Q_OVERFLOW | FS_IN_IGNORED)


#define ALL_FSNOTIFY_FLAGS  (FS_EXCL_UNLINK | FS_ISDIR | FS_IN_ONESHOT | \
			     FS_DN_MULTISHOT | FS_EVENT_ON_CHILD)

#define ALL_FSNOTIFY_BITS   (ALL_FSNOTIFY_EVENTS | ALL_FSNOTIFY_FLAGS)

struct fsnotify_group;
struct fsnotify_event;
struct fsnotify_mark;
struct fsnotify_event_private_data;
struct fsnotify_fname;
struct fsnotify_iter_info;

struct mem_cgroup;


struct fsnotify_ops {
	int (*handle_event)(struct fsnotify_group *group, u32 mask,
			    const void *data, int data_type, struct inode *dir,
			    const struct qstr *file_name, u32 cookie,
			    struct fsnotify_iter_info *iter_info);
	int (*handle_inode_event)(struct fsnotify_mark *mark, u32 mask,
			    struct inode *inode, struct inode *dir,
			    const struct qstr *file_name, u32 cookie);
	void (*free_group_priv)(struct fsnotify_group *group);
	void (*freeing_mark)(struct fsnotify_mark *mark, struct fsnotify_group *group);
	void (*free_event)(struct fsnotify_event *event);
	
	void (*free_mark)(struct fsnotify_mark *mark);
};


struct fsnotify_event {
	struct list_head list;
	unsigned long objectid;	
};


struct fsnotify_group {
	const struct fsnotify_ops *ops;	

	
	refcount_t refcnt;		

	
	spinlock_t notification_lock;		
	struct list_head notification_list;	
	wait_queue_head_t notification_waitq;	
	unsigned int q_len;			
	unsigned int max_events;		
	
	#define FS_PRIO_0	0 
	#define FS_PRIO_1	1 
	#define FS_PRIO_2	2 
	unsigned int priority;
	bool shutdown;		

	
	struct mutex mark_mutex;	
	atomic_t num_marks;		
	atomic_t user_waits;		
	struct list_head marks_list;	

	struct fasync_struct *fsn_fa;    

	struct fsnotify_event *overflow_event;	

	struct mem_cgroup *memcg;	

	
	union {
		void *private;
#ifdef CONFIG_INOTIFY_USER
		struct inotify_group_private_data {
			spinlock_t	idr_lock;
			struct idr      idr;
			struct ucounts *ucounts;
		} inotify_data;
#endif
#ifdef CONFIG_FANOTIFY
		struct fanotify_group_private_data {
			
			struct list_head access_list;
			wait_queue_head_t access_waitq;
			int flags;           
			int f_flags; 
			unsigned int max_marks;
			struct user_struct *user;
		} fanotify_data;
#endif 
	};
};


enum fsnotify_data_type {
	FSNOTIFY_EVENT_NONE,
	FSNOTIFY_EVENT_PATH,
	FSNOTIFY_EVENT_INODE,
};

static inline struct inode *fsnotify_data_inode(const void *data, int data_type)
{
	switch (data_type) {
	case FSNOTIFY_EVENT_INODE:
		return (struct inode *)data;
	case FSNOTIFY_EVENT_PATH:
		return d_inode(((const struct path *)data)->dentry);
	default:
		return NULL;
	}
}

static inline const struct path *fsnotify_data_path(const void *data,
						    int data_type)
{
	switch (data_type) {
	case FSNOTIFY_EVENT_PATH:
		return data;
	default:
		return NULL;
	}
}

enum fsnotify_obj_type {
	FSNOTIFY_OBJ_TYPE_INODE,
	FSNOTIFY_OBJ_TYPE_PARENT,
	FSNOTIFY_OBJ_TYPE_VFSMOUNT,
	FSNOTIFY_OBJ_TYPE_SB,
	FSNOTIFY_OBJ_TYPE_COUNT,
	FSNOTIFY_OBJ_TYPE_DETACHED = FSNOTIFY_OBJ_TYPE_COUNT
};

#define FSNOTIFY_OBJ_TYPE_INODE_FL	(1U << FSNOTIFY_OBJ_TYPE_INODE)
#define FSNOTIFY_OBJ_TYPE_PARENT_FL	(1U << FSNOTIFY_OBJ_TYPE_PARENT)
#define FSNOTIFY_OBJ_TYPE_VFSMOUNT_FL	(1U << FSNOTIFY_OBJ_TYPE_VFSMOUNT)
#define FSNOTIFY_OBJ_TYPE_SB_FL		(1U << FSNOTIFY_OBJ_TYPE_SB)
#define FSNOTIFY_OBJ_ALL_TYPES_MASK	((1U << FSNOTIFY_OBJ_TYPE_COUNT) - 1)

static inline bool fsnotify_valid_obj_type(unsigned int type)
{
	return (type < FSNOTIFY_OBJ_TYPE_COUNT);
}

struct fsnotify_iter_info {
	struct fsnotify_mark *marks[FSNOTIFY_OBJ_TYPE_COUNT];
	unsigned int report_mask;
	int srcu_idx;
};

static inline bool fsnotify_iter_should_report_type(
		struct fsnotify_iter_info *iter_info, int type)
{
	return (iter_info->report_mask & (1U << type));
}

static inline void fsnotify_iter_set_report_type(
		struct fsnotify_iter_info *iter_info, int type)
{
	iter_info->report_mask |= (1U << type);
}

static inline void fsnotify_iter_set_report_type_mark(
		struct fsnotify_iter_info *iter_info, int type,
		struct fsnotify_mark *mark)
{
	iter_info->marks[type] = mark;
	iter_info->report_mask |= (1U << type);
}

#define FSNOTIFY_ITER_FUNCS(name, NAME) \
static inline struct fsnotify_mark *fsnotify_iter_##name##_mark( \
		struct fsnotify_iter_info *iter_info) \
{ \
	return (iter_info->report_mask & FSNOTIFY_OBJ_TYPE_##NAME##_FL) ? \
		iter_info->marks[FSNOTIFY_OBJ_TYPE_##NAME] : NULL; \
}

FSNOTIFY_ITER_FUNCS(inode, INODE)
FSNOTIFY_ITER_FUNCS(parent, PARENT)
FSNOTIFY_ITER_FUNCS(vfsmount, VFSMOUNT)
FSNOTIFY_ITER_FUNCS(sb, SB)

#define fsnotify_foreach_obj_type(type) \
	for (type = 0; type < FSNOTIFY_OBJ_TYPE_COUNT; type++)


struct fsnotify_mark_connector;
typedef struct fsnotify_mark_connector __rcu *fsnotify_connp_t;


struct fsnotify_mark_connector {
	spinlock_t lock;
	unsigned short type;	
#define FSNOTIFY_CONN_FLAG_HAS_FSID	0x01
	unsigned short flags;	
	__kernel_fsid_t fsid;	
	union {
		
		fsnotify_connp_t *obj;
		
		struct fsnotify_mark_connector *destroy_next;
	};
	struct hlist_head list;
};


struct fsnotify_mark {
	
	__u32 mask;
	
	refcount_t refcnt;
	
	struct fsnotify_group *group;
	
	struct list_head g_list;
	
	spinlock_t lock;
	
	struct hlist_node obj_list;
	
	struct fsnotify_mark_connector *connector;
	
	__u32 ignored_mask;
#define FSNOTIFY_MARK_FLAG_IGNORED_SURV_MODIFY	0x01
#define FSNOTIFY_MARK_FLAG_ALIVE		0x02
#define FSNOTIFY_MARK_FLAG_ATTACHED		0x04
	unsigned int flags;		
};

#ifdef CONFIG_FSNOTIFY




extern int fsnotify(__u32 mask, const void *data, int data_type,
		    struct inode *dir, const struct qstr *name,
		    struct inode *inode, u32 cookie);
extern int __fsnotify_parent(struct dentry *dentry, __u32 mask, const void *data,
			   int data_type);
extern void __fsnotify_inode_delete(struct inode *inode);
extern void __fsnotify_vfsmount_delete(struct vfsmount *mnt);
extern void fsnotify_sb_delete(struct super_block *sb);
extern u32 fsnotify_get_cookie(void);

static inline __u32 fsnotify_parent_needed_mask(__u32 mask)
{
	
	if (!(mask & FS_EVENT_ON_CHILD))
		return 0;
	
	return mask & FS_EVENTS_POSS_TO_PARENT;
}

static inline int fsnotify_inode_watches_children(struct inode *inode)
{
	
	if (!(inode->i_fsnotify_mask & FS_EVENT_ON_CHILD))
		return 0;
	
	return inode->i_fsnotify_mask & FS_EVENTS_POSS_ON_CHILD;
}


static inline void fsnotify_update_flags(struct dentry *dentry)
{
	assert_spin_locked(&dentry->d_lock);

	
	if (fsnotify_inode_watches_children(dentry->d_parent->d_inode))
		dentry->d_flags |= DCACHE_FSNOTIFY_PARENT_WATCHED;
	else
		dentry->d_flags &= ~DCACHE_FSNOTIFY_PARENT_WATCHED;
}




extern struct fsnotify_group *fsnotify_alloc_group(const struct fsnotify_ops *ops);

extern void fsnotify_get_group(struct fsnotify_group *group);

extern void fsnotify_put_group(struct fsnotify_group *group);

extern void fsnotify_group_stop_queueing(struct fsnotify_group *group);

extern void fsnotify_destroy_group(struct fsnotify_group *group);

extern int fsnotify_fasync(int fd, struct file *file, int on);

extern void fsnotify_destroy_event(struct fsnotify_group *group,
				   struct fsnotify_event *event);

extern int fsnotify_add_event(struct fsnotify_group *group,
			      struct fsnotify_event *event,
			      int (*merge)(struct list_head *,
					   struct fsnotify_event *));

static inline void fsnotify_queue_overflow(struct fsnotify_group *group)
{
	fsnotify_add_event(group, group->overflow_event, NULL);
}


extern bool fsnotify_notify_queue_is_empty(struct fsnotify_group *group);

extern struct fsnotify_event *fsnotify_peek_first_event(struct fsnotify_group *group);

extern struct fsnotify_event *fsnotify_remove_first_event(struct fsnotify_group *group);

extern void fsnotify_remove_queued_event(struct fsnotify_group *group,
					 struct fsnotify_event *event);




extern __u32 fsnotify_conn_mask(struct fsnotify_mark_connector *conn);

extern void fsnotify_recalc_mask(struct fsnotify_mark_connector *conn);
extern void fsnotify_init_mark(struct fsnotify_mark *mark,
			       struct fsnotify_group *group);

extern struct fsnotify_mark *fsnotify_find_mark(fsnotify_connp_t *connp,
						struct fsnotify_group *group);

extern int fsnotify_get_conn_fsid(const struct fsnotify_mark_connector *conn,
				  __kernel_fsid_t *fsid);

extern int fsnotify_add_mark(struct fsnotify_mark *mark,
			     fsnotify_connp_t *connp, unsigned int type,
			     int allow_dups, __kernel_fsid_t *fsid);
extern int fsnotify_add_mark_locked(struct fsnotify_mark *mark,
				    fsnotify_connp_t *connp,
				    unsigned int type, int allow_dups,
				    __kernel_fsid_t *fsid);


static inline int fsnotify_add_inode_mark(struct fsnotify_mark *mark,
					  struct inode *inode,
					  int allow_dups)
{
	return fsnotify_add_mark(mark, &inode->i_fsnotify_marks,
				 FSNOTIFY_OBJ_TYPE_INODE, allow_dups, NULL);
}
static inline int fsnotify_add_inode_mark_locked(struct fsnotify_mark *mark,
						 struct inode *inode,
						 int allow_dups)
{
	return fsnotify_add_mark_locked(mark, &inode->i_fsnotify_marks,
					FSNOTIFY_OBJ_TYPE_INODE, allow_dups,
					NULL);
}


extern void fsnotify_destroy_mark(struct fsnotify_mark *mark,
				  struct fsnotify_group *group);

extern void fsnotify_detach_mark(struct fsnotify_mark *mark);

extern void fsnotify_free_mark(struct fsnotify_mark *mark);

extern void fsnotify_wait_marks_destroyed(void);

extern void fsnotify_clear_marks_by_group(struct fsnotify_group *group, unsigned int type);

static inline void fsnotify_clear_vfsmount_marks_by_group(struct fsnotify_group *group)
{
	fsnotify_clear_marks_by_group(group, FSNOTIFY_OBJ_TYPE_VFSMOUNT_FL);
}

static inline void fsnotify_clear_inode_marks_by_group(struct fsnotify_group *group)
{
	fsnotify_clear_marks_by_group(group, FSNOTIFY_OBJ_TYPE_INODE_FL);
}

static inline void fsnotify_clear_sb_marks_by_group(struct fsnotify_group *group)
{
	fsnotify_clear_marks_by_group(group, FSNOTIFY_OBJ_TYPE_SB_FL);
}
extern void fsnotify_get_mark(struct fsnotify_mark *mark);
extern void fsnotify_put_mark(struct fsnotify_mark *mark);
extern void fsnotify_finish_user_wait(struct fsnotify_iter_info *iter_info);
extern bool fsnotify_prepare_user_wait(struct fsnotify_iter_info *iter_info);

static inline void fsnotify_init_event(struct fsnotify_event *event,
				       unsigned long objectid)
{
	INIT_LIST_HEAD(&event->list);
	event->objectid = objectid;
}

#else

static inline int fsnotify(__u32 mask, const void *data, int data_type,
			   struct inode *dir, const struct qstr *name,
			   struct inode *inode, u32 cookie)
{
	return 0;
}

static inline int __fsnotify_parent(struct dentry *dentry, __u32 mask,
				  const void *data, int data_type)
{
	return 0;
}

static inline void __fsnotify_inode_delete(struct inode *inode)
{}

static inline void __fsnotify_vfsmount_delete(struct vfsmount *mnt)
{}

static inline void fsnotify_sb_delete(struct super_block *sb)
{}

static inline void fsnotify_update_flags(struct dentry *dentry)
{}

static inline u32 fsnotify_get_cookie(void)
{
	return 0;
}

static inline void fsnotify_unmount_inodes(struct super_block *sb)
{}

#endif	

#endif	

#endif	
