/* SPDX-License-Identifier: GPL-2.0
 * Copyright (c) 2019, Vladimir Oltean <olteanv@gmail.com>
 */



#ifndef _NET_DSA_SJA1105_H
#define _NET_DSA_SJA1105_H

#include <linux/skbuff.h>
#include <linux/etherdevice.h>
#include <linux/dsa/8021q.h>
#include <net/dsa.h>

#define ETH_P_SJA1105				ETH_P_DSA_8021Q
#define ETH_P_SJA1105_META			0x0008


#define SJA1105_LINKLOCAL_FILTER_A		0x0180C2000000ull
#define SJA1105_LINKLOCAL_FILTER_A_MASK		0xFFFFFF000000ull

#define SJA1105_LINKLOCAL_FILTER_B		0x011B19000000ull
#define SJA1105_LINKLOCAL_FILTER_B_MASK		0xFFFFFF000000ull


#define SJA1105_META_SMAC			0x222222222222ull
#define SJA1105_META_DMAC			0x0180C200000Eull

#define SJA1105_HWTS_RX_EN			0


struct sja1105_tagger_data {
	struct sk_buff *stampable_skb;
	
	spinlock_t meta_lock;
	unsigned long state;
};

struct sja1105_skb_cb {
	u32 meta_tstamp;
};

#define SJA1105_SKB_CB(skb) \
	((struct sja1105_skb_cb *)DSA_SKB_CB_PRIV(skb))

struct sja1105_port {
	u16 subvlan_map[DSA_8021Q_N_SUBVLAN];
	struct kthread_worker *xmit_worker;
	struct kthread_work xmit_work;
	struct sk_buff_head xmit_queue;
	struct sja1105_tagger_data *data;
	struct dsa_port *dp;
	bool hwts_tx_en;
	u16 xmit_tpid;
};

#endif 
