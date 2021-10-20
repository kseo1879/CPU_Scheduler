/* SPDX-License-Identifier: GPL-2.0+ */


#ifndef __INCLUDE_LINUX_RCU_SEGCBLIST_H
#define __INCLUDE_LINUX_RCU_SEGCBLIST_H

#include <linux/types.h>
#include <linux/atomic.h>


struct rcu_cblist {
	struct rcu_head *head;
	struct rcu_head **tail;
	long len;
};

#define RCU_CBLIST_INITIALIZER(n) { .head = NULL, .tail = &n.head }




#define RCU_DONE_TAIL		0	
#define RCU_WAIT_TAIL		1	
#define RCU_NEXT_READY_TAIL	2	
#define RCU_NEXT_TAIL		3
#define RCU_CBLIST_NSEGS	4

struct rcu_segcblist {
	struct rcu_head *head;
	struct rcu_head **tails[RCU_CBLIST_NSEGS];
	unsigned long gp_seq[RCU_CBLIST_NSEGS];
#ifdef CONFIG_RCU_NOCB_CPU
	atomic_long_t len;
#else
	long len;
#endif
	u8 enabled;
	u8 offloaded;
};

#define RCU_SEGCBLIST_INITIALIZER(n) \
{ \
	.head = NULL, \
	.tails[RCU_DONE_TAIL] = &n.head, \
	.tails[RCU_WAIT_TAIL] = &n.head, \
	.tails[RCU_NEXT_READY_TAIL] = &n.head, \
	.tails[RCU_NEXT_TAIL] = &n.head, \
}

#endif 
