/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_FS_NOTIFY_H
#define _LINUX_FS_NOTIFY_H



#include <linux/fsnotify_backend.h>
#include <linux/audit.h>
#include <linux/slab.h>
#include <linux/bug.h>


static inline void fsnotify_name(struct inode *dir, __u32 mask,
				 struct inode *child,
				 const struct qstr *name, u32 cookie)
{
	fsnotify(mask, child, FSNOTIFY_EVENT_INODE, dir, name, NULL, cookie);
}

static inline void fsnotify_dirent(struct inode *dir, struct dentry *dentry,
				   __u32 mask)
{
	fsnotify_name(dir, mask, d_inode(dentry), &dentry->d_name, 0);
}

static inline void fsnotify_inode(struct inode *inode, __u32 mask)
{
	if (S_ISDIR(inode->i_mode))
		mask |= FS_ISDIR;

	fsnotify(mask, inode, FSNOTIFY_EVENT_INODE, NULL, NULL, inode, 0);
}


static inline int fsnotify_parent(struct dentry *dentry, __u32 mask,
				  const void *data, int data_type)
{
	struct inode *inode = d_inode(dentry);

	if (S_ISDIR(inode->i_mode)) {
		mask |= FS_ISDIR;

		
		if (!(dentry->d_flags & DCACHE_FSNOTIFY_PARENT_WATCHED))
			goto notify_child;
	}

	
	if (IS_ROOT(dentry))
		goto notify_child;

	return __fsnotify_parent(dentry, mask, data, data_type);

notify_child:
	return fsnotify(mask, data, data_type, NULL, NULL, inode, 0);
}


static inline void fsnotify_dentry(struct dentry *dentry, __u32 mask)
{
	fsnotify_parent(dentry, mask, d_inode(dentry), FSNOTIFY_EVENT_INODE);
}

static inline int fsnotify_file(struct file *file, __u32 mask)
{
	const struct path *path = &file->f_path;

	if (file->f_mode & FMODE_NONOTIFY)
		return 0;

	return fsnotify_parent(path->dentry, mask, path, FSNOTIFY_EVENT_PATH);
}


static inline int fsnotify_perm(struct file *file, int mask)
{
	int ret;
	__u32 fsnotify_mask = 0;

	if (!(mask & (MAY_READ | MAY_OPEN)))
		return 0;

	if (mask & MAY_OPEN) {
		fsnotify_mask = FS_OPEN_PERM;

		if (file->f_flags & __FMODE_EXEC) {
			ret = fsnotify_file(file, FS_OPEN_EXEC_PERM);

			if (ret)
				return ret;
		}
	} else if (mask & MAY_READ) {
		fsnotify_mask = FS_ACCESS_PERM;
	}

	return fsnotify_file(file, fsnotify_mask);
}


static inline void fsnotify_link_count(struct inode *inode)
{
	fsnotify_inode(inode, FS_ATTRIB);
}


static inline void fsnotify_move(struct inode *old_dir, struct inode *new_dir,
				 const struct qstr *old_name,
				 int isdir, struct inode *target,
				 struct dentry *moved)
{
	struct inode *source = moved->d_inode;
	u32 fs_cookie = fsnotify_get_cookie();
	__u32 old_dir_mask = FS_MOVED_FROM;
	__u32 new_dir_mask = FS_MOVED_TO;
	const struct qstr *new_name = &moved->d_name;

	if (old_dir == new_dir)
		old_dir_mask |= FS_DN_RENAME;

	if (isdir) {
		old_dir_mask |= FS_ISDIR;
		new_dir_mask |= FS_ISDIR;
	}

	fsnotify_name(old_dir, old_dir_mask, source, old_name, fs_cookie);
	fsnotify_name(new_dir, new_dir_mask, source, new_name, fs_cookie);

	if (target)
		fsnotify_link_count(target);
	fsnotify_inode(source, FS_MOVE_SELF);
	audit_inode_child(new_dir, moved, AUDIT_TYPE_CHILD_CREATE);
}


static inline void fsnotify_inode_delete(struct inode *inode)
{
	__fsnotify_inode_delete(inode);
}


static inline void fsnotify_vfsmount_delete(struct vfsmount *mnt)
{
	__fsnotify_vfsmount_delete(mnt);
}


static inline void fsnotify_inoderemove(struct inode *inode)
{
	fsnotify_inode(inode, FS_DELETE_SELF);
	__fsnotify_inode_delete(inode);
}


static inline void fsnotify_create(struct inode *inode, struct dentry *dentry)
{
	audit_inode_child(inode, dentry, AUDIT_TYPE_CHILD_CREATE);

	fsnotify_dirent(inode, dentry, FS_CREATE);
}


static inline void fsnotify_link(struct inode *dir, struct inode *inode,
				 struct dentry *new_dentry)
{
	fsnotify_link_count(inode);
	audit_inode_child(dir, new_dentry, AUDIT_TYPE_CHILD_CREATE);

	fsnotify_name(dir, FS_CREATE, inode, &new_dentry->d_name, 0);
}


static inline void fsnotify_unlink(struct inode *dir, struct dentry *dentry)
{
	
	WARN_ON_ONCE(d_is_negative(dentry));

	fsnotify_dirent(dir, dentry, FS_DELETE);
}


static inline void fsnotify_mkdir(struct inode *inode, struct dentry *dentry)
{
	audit_inode_child(inode, dentry, AUDIT_TYPE_CHILD_CREATE);

	fsnotify_dirent(inode, dentry, FS_CREATE | FS_ISDIR);
}


static inline void fsnotify_rmdir(struct inode *dir, struct dentry *dentry)
{
	
	WARN_ON_ONCE(d_is_negative(dentry));

	fsnotify_dirent(dir, dentry, FS_DELETE | FS_ISDIR);
}


static inline void fsnotify_access(struct file *file)
{
	fsnotify_file(file, FS_ACCESS);
}


static inline void fsnotify_modify(struct file *file)
{
	fsnotify_file(file, FS_MODIFY);
}


static inline void fsnotify_open(struct file *file)
{
	__u32 mask = FS_OPEN;

	if (file->f_flags & __FMODE_EXEC)
		mask |= FS_OPEN_EXEC;

	fsnotify_file(file, mask);
}


static inline void fsnotify_close(struct file *file)
{
	__u32 mask = (file->f_mode & FMODE_WRITE) ? FS_CLOSE_WRITE :
						    FS_CLOSE_NOWRITE;

	fsnotify_file(file, mask);
}


static inline void fsnotify_xattr(struct dentry *dentry)
{
	fsnotify_dentry(dentry, FS_ATTRIB);
}


static inline void fsnotify_change(struct dentry *dentry, unsigned int ia_valid)
{
	__u32 mask = 0;

	if (ia_valid & ATTR_UID)
		mask |= FS_ATTRIB;
	if (ia_valid & ATTR_GID)
		mask |= FS_ATTRIB;
	if (ia_valid & ATTR_SIZE)
		mask |= FS_MODIFY;

	
	if ((ia_valid & (ATTR_ATIME | ATTR_MTIME)) == (ATTR_ATIME | ATTR_MTIME))
		mask |= FS_ATTRIB;
	else if (ia_valid & ATTR_ATIME)
		mask |= FS_ACCESS;
	else if (ia_valid & ATTR_MTIME)
		mask |= FS_MODIFY;

	if (ia_valid & ATTR_MODE)
		mask |= FS_ATTRIB;

	if (mask)
		fsnotify_dentry(dentry, mask);
}

#endif	
