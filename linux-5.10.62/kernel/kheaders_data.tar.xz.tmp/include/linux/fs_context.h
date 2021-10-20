/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _LINUX_FS_CONTEXT_H
#define _LINUX_FS_CONTEXT_H

#include <linux/kernel.h>
#include <linux/refcount.h>
#include <linux/errno.h>
#include <linux/security.h>
#include <linux/mutex.h>

struct cred;
struct dentry;
struct file_operations;
struct file_system_type;
struct mnt_namespace;
struct net;
struct pid_namespace;
struct super_block;
struct user_namespace;
struct vfsmount;
struct path;

enum fs_context_purpose {
	FS_CONTEXT_FOR_MOUNT,		
	FS_CONTEXT_FOR_SUBMOUNT,	
	FS_CONTEXT_FOR_RECONFIGURE,	
};


enum fs_context_phase {
	FS_CONTEXT_CREATE_PARAMS,	
	FS_CONTEXT_CREATING,		
	FS_CONTEXT_AWAITING_MOUNT,	
	FS_CONTEXT_AWAITING_RECONF,	
	FS_CONTEXT_RECONF_PARAMS,	
	FS_CONTEXT_RECONFIGURING,	
	FS_CONTEXT_FAILED,		
};


enum fs_value_type {
	fs_value_is_undefined,
	fs_value_is_flag,		
	fs_value_is_string,		
	fs_value_is_blob,		
	fs_value_is_filename,		
	fs_value_is_file,		
};


struct fs_parameter {
	const char		*key;		
	enum fs_value_type	type:8;		
	union {
		char		*string;
		void		*blob;
		struct filename	*name;
		struct file	*file;
	};
	size_t	size;
	int	dirfd;
};

struct p_log {
	const char *prefix;
	struct fc_log *log;
};


struct fs_context {
	const struct fs_context_operations *ops;
	struct mutex		uapi_mutex;	
	struct file_system_type	*fs_type;
	void			*fs_private;	
	void			*sget_key;
	struct dentry		*root;		
	struct user_namespace	*user_ns;	
	struct net		*net_ns;	
	const struct cred	*cred;		
	struct p_log		log;		
	const char		*source;	
	void			*security;	
	void			*s_fs_info;	
	unsigned int		sb_flags;	
	unsigned int		sb_flags_mask;	
	unsigned int		s_iflags;	
	unsigned int		lsm_flags;	
	enum fs_context_purpose	purpose:8;
	enum fs_context_phase	phase:8;	
	bool			need_free:1;	
	bool			global:1;	
	bool			oldapi:1;	
};

struct fs_context_operations {
	void (*free)(struct fs_context *fc);
	int (*dup)(struct fs_context *fc, struct fs_context *src_fc);
	int (*parse_param)(struct fs_context *fc, struct fs_parameter *param);
	int (*parse_monolithic)(struct fs_context *fc, void *data);
	int (*get_tree)(struct fs_context *fc);
	int (*reconfigure)(struct fs_context *fc);
};


extern struct fs_context *fs_context_for_mount(struct file_system_type *fs_type,
						unsigned int sb_flags);
extern struct fs_context *fs_context_for_reconfigure(struct dentry *dentry,
						unsigned int sb_flags,
						unsigned int sb_flags_mask);
extern struct fs_context *fs_context_for_submount(struct file_system_type *fs_type,
						struct dentry *reference);

extern struct fs_context *vfs_dup_fs_context(struct fs_context *fc);
extern int vfs_parse_fs_param(struct fs_context *fc, struct fs_parameter *param);
extern int vfs_parse_fs_string(struct fs_context *fc, const char *key,
			       const char *value, size_t v_size);
extern int generic_parse_monolithic(struct fs_context *fc, void *data);
extern int vfs_get_tree(struct fs_context *fc);
extern void put_fs_context(struct fs_context *fc);
extern void fc_drop_locked(struct fs_context *fc);


enum vfs_get_super_keying {
	vfs_get_single_super,	
	vfs_get_single_reconf_super, 
	vfs_get_keyed_super,	
	vfs_get_independent_super, 
};
extern int vfs_get_super(struct fs_context *fc,
			 enum vfs_get_super_keying keying,
			 int (*fill_super)(struct super_block *sb,
					   struct fs_context *fc));

extern int get_tree_nodev(struct fs_context *fc,
			 int (*fill_super)(struct super_block *sb,
					   struct fs_context *fc));
extern int get_tree_single(struct fs_context *fc,
			 int (*fill_super)(struct super_block *sb,
					   struct fs_context *fc));
extern int get_tree_single_reconf(struct fs_context *fc,
			 int (*fill_super)(struct super_block *sb,
					   struct fs_context *fc));
extern int get_tree_keyed(struct fs_context *fc,
			 int (*fill_super)(struct super_block *sb,
					   struct fs_context *fc),
			 void *key);

extern int get_tree_bdev(struct fs_context *fc,
			       int (*fill_super)(struct super_block *sb,
						 struct fs_context *fc));

extern const struct file_operations fscontext_fops;


struct fc_log {
	refcount_t	usage;
	u8		head;		
	u8		tail;		
	u8		need_free;	
	struct module	*owner;		
	char		*buffer[8];
};

extern __attribute__((format(printf, 4, 5)))
void logfc(struct fc_log *log, const char *prefix, char level, const char *fmt, ...);

#define __logfc(fc, l, fmt, ...) logfc((fc)->log.log, NULL, \
					l, fmt, ## __VA_ARGS__)
#define __plog(p, l, fmt, ...) logfc((p)->log, (p)->prefix, \
					l, fmt, ## __VA_ARGS__)

#define infof(fc, fmt, ...) __logfc(fc, 'i', fmt, ## __VA_ARGS__)
#define info_plog(p, fmt, ...) __plog(p, 'i', fmt, ## __VA_ARGS__)
#define infofc(p, fmt, ...) __plog((&(fc)->log), 'i', fmt, ## __VA_ARGS__)


#define warnf(fc, fmt, ...) __logfc(fc, 'w', fmt, ## __VA_ARGS__)
#define warn_plog(p, fmt, ...) __plog(p, 'w', fmt, ## __VA_ARGS__)
#define warnfc(fc, fmt, ...) __plog((&(fc)->log), 'w', fmt, ## __VA_ARGS__)


#define errorf(fc, fmt, ...) __logfc(fc, 'e', fmt, ## __VA_ARGS__)
#define error_plog(p, fmt, ...) __plog(p, 'e', fmt, ## __VA_ARGS__)
#define errorfc(fc, fmt, ...) __plog((&(fc)->log), 'e', fmt, ## __VA_ARGS__)


#define invalf(fc, fmt, ...) (errorf(fc, fmt, ## __VA_ARGS__), -EINVAL)
#define inval_plog(p, fmt, ...) (error_plog(p, fmt, ## __VA_ARGS__), -EINVAL)
#define invalfc(fc, fmt, ...) (errorfc(fc, fmt, ## __VA_ARGS__), -EINVAL)

#endif 
