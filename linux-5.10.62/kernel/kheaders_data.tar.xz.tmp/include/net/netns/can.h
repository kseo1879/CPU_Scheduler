/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __NETNS_CAN_H__
#define __NETNS_CAN_H__

#include <linux/spinlock.h>

struct can_dev_rcv_lists;
struct can_pkg_stats;
struct can_rcv_lists_stats;

struct netns_can {
#if IS_ENABLED(CONFIG_PROC_FS)
	struct proc_dir_entry *proc_dir;
	struct proc_dir_entry *pde_stats;
	struct proc_dir_entry *pde_reset_stats;
	struct proc_dir_entry *pde_rcvlist_all;
	struct proc_dir_entry *pde_rcvlist_fil;
	struct proc_dir_entry *pde_rcvlist_inv;
	struct proc_dir_entry *pde_rcvlist_sff;
	struct proc_dir_entry *pde_rcvlist_eff;
	struct proc_dir_entry *pde_rcvlist_err;
	struct proc_dir_entry *bcmproc_dir;
#endif

	
	struct can_dev_rcv_lists *rx_alldev_list;
	spinlock_t rcvlists_lock;
	struct timer_list stattimer; 
	struct can_pkg_stats *pkg_stats;
	struct can_rcv_lists_stats *rcv_lists_stats;

	
	struct hlist_head cgw_list;
};

#endif 
