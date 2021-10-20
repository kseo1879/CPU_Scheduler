/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __MTK_CMDQ_H__
#define __MTK_CMDQ_H__

#include <linux/mailbox_client.h>
#include <linux/mailbox/mtk-cmdq-mailbox.h>
#include <linux/timer.h>

#define CMDQ_NO_TIMEOUT		0xffffffffu
#define CMDQ_ADDR_HIGH(addr)	((u32)(((addr) >> 16) & GENMASK(31, 0)))
#define CMDQ_ADDR_LOW(addr)	((u16)(addr) | BIT(1))

struct cmdq_pkt;

struct cmdq_client_reg {
	u8 subsys;
	u16 offset;
	u16 size;
};

struct cmdq_client {
	spinlock_t lock;
	u32 pkt_cnt;
	struct mbox_client client;
	struct mbox_chan *chan;
	struct timer_list timer;
	u32 timeout_ms; 
};


int cmdq_dev_get_client_reg(struct device *dev,
			    struct cmdq_client_reg *client_reg, int idx);


struct cmdq_client *cmdq_mbox_create(struct device *dev, int index,
				     u32 timeout);


void cmdq_mbox_destroy(struct cmdq_client *client);


struct cmdq_pkt *cmdq_pkt_create(struct cmdq_client *client, size_t size);


void cmdq_pkt_destroy(struct cmdq_pkt *pkt);


int cmdq_pkt_write(struct cmdq_pkt *pkt, u8 subsys, u16 offset, u32 value);


int cmdq_pkt_write_mask(struct cmdq_pkt *pkt, u8 subsys,
			u16 offset, u32 value, u32 mask);


int cmdq_pkt_read_s(struct cmdq_pkt *pkt, u16 high_addr_reg_idx, u16 addr_low,
		    u16 reg_idx);


int cmdq_pkt_write_s(struct cmdq_pkt *pkt, u16 high_addr_reg_idx,
		     u16 addr_low, u16 src_reg_idx);


int cmdq_pkt_write_s_mask(struct cmdq_pkt *pkt, u16 high_addr_reg_idx,
			  u16 addr_low, u16 src_reg_idx, u32 mask);


int cmdq_pkt_write_s_value(struct cmdq_pkt *pkt, u8 high_addr_reg_idx,
			   u16 addr_low, u32 value);


int cmdq_pkt_write_s_mask_value(struct cmdq_pkt *pkt, u8 high_addr_reg_idx,
				u16 addr_low, u32 value, u32 mask);


int cmdq_pkt_wfe(struct cmdq_pkt *pkt, u16 event, bool clear);


int cmdq_pkt_clear_event(struct cmdq_pkt *pkt, u16 event);


int cmdq_pkt_set_event(struct cmdq_pkt *pkt, u16 event);


int cmdq_pkt_poll(struct cmdq_pkt *pkt, u8 subsys,
		  u16 offset, u32 value);


int cmdq_pkt_poll_mask(struct cmdq_pkt *pkt, u8 subsys,
		       u16 offset, u32 value, u32 mask);


int cmdq_pkt_assign(struct cmdq_pkt *pkt, u16 reg_idx, u32 value);


int cmdq_pkt_jump(struct cmdq_pkt *pkt, dma_addr_t addr);


int cmdq_pkt_finalize(struct cmdq_pkt *pkt);


int cmdq_pkt_flush_async(struct cmdq_pkt *pkt, cmdq_async_flush_cb cb,
			 void *data);


int cmdq_pkt_flush(struct cmdq_pkt *pkt);

#endif	
