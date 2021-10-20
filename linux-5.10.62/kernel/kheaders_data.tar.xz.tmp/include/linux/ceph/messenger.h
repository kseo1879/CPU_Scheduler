/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __FS_CEPH_MESSENGER_H
#define __FS_CEPH_MESSENGER_H

#include <linux/bvec.h>
#include <linux/kref.h>
#include <linux/mutex.h>
#include <linux/net.h>
#include <linux/radix-tree.h>
#include <linux/uio.h>
#include <linux/workqueue.h>
#include <net/net_namespace.h>

#include <linux/ceph/types.h>
#include <linux/ceph/buffer.h>

struct ceph_msg;
struct ceph_connection;


struct ceph_connection_operations {
	struct ceph_connection *(*get)(struct ceph_connection *);
	void (*put)(struct ceph_connection *);

	
	void (*dispatch) (struct ceph_connection *con, struct ceph_msg *m);

	
	struct ceph_auth_handshake *(*get_authorizer) (
				struct ceph_connection *con,
			       int *proto, int force_new);
	int (*add_authorizer_challenge)(struct ceph_connection *con,
					void *challenge_buf,
					int challenge_buf_len);
	int (*verify_authorizer_reply) (struct ceph_connection *con);
	int (*invalidate_authorizer)(struct ceph_connection *con);

	
	void (*fault) (struct ceph_connection *con);

	
	void (*peer_reset) (struct ceph_connection *con);

	struct ceph_msg * (*alloc_msg) (struct ceph_connection *con,
					struct ceph_msg_header *hdr,
					int *skip);

	void (*reencode_message) (struct ceph_msg *msg);

	int (*sign_message) (struct ceph_msg *msg);
	int (*check_message_signature) (struct ceph_msg *msg);
};


#define ENTITY_NAME(n) ceph_entity_type_name((n).type), le64_to_cpu((n).num)

struct ceph_messenger {
	struct ceph_entity_inst inst;    
	struct ceph_entity_addr my_enc_addr;

	atomic_t stopping;
	possible_net_t net;

	
	u32 global_seq;
	spinlock_t global_seq_lock;
};

enum ceph_msg_data_type {
	CEPH_MSG_DATA_NONE,	
	CEPH_MSG_DATA_PAGES,	
	CEPH_MSG_DATA_PAGELIST,	
#ifdef CONFIG_BLOCK
	CEPH_MSG_DATA_BIO,	
#endif 
	CEPH_MSG_DATA_BVECS,	
};

#ifdef CONFIG_BLOCK

struct ceph_bio_iter {
	struct bio *bio;
	struct bvec_iter iter;
};

#define __ceph_bio_iter_advance_step(it, n, STEP) do {			      \
	unsigned int __n = (n), __cur_n;				      \
									      \
	while (__n) {							      \
		BUG_ON(!(it)->iter.bi_size);				      \
		__cur_n = min((it)->iter.bi_size, __n);			      \
		(void)(STEP);						      \
		bio_advance_iter((it)->bio, &(it)->iter, __cur_n);	      \
		if (!(it)->iter.bi_size && (it)->bio->bi_next) {	      \
			dout("__ceph_bio_iter_advance_step next bio\n");      \
			(it)->bio = (it)->bio->bi_next;			      \
			(it)->iter = (it)->bio->bi_iter;		      \
		}							      \
		__n -= __cur_n;						      \
	}								      \
} while (0)


#define ceph_bio_iter_advance(it, n)					      \
	__ceph_bio_iter_advance_step(it, n, 0)


#define ceph_bio_iter_advance_step(it, n, BVEC_STEP)			      \
	__ceph_bio_iter_advance_step(it, n, ({				      \
		struct bio_vec bv;					      \
		struct bvec_iter __cur_iter;				      \
									      \
		__cur_iter = (it)->iter;				      \
		__cur_iter.bi_size = __cur_n;				      \
		__bio_for_each_segment(bv, (it)->bio, __cur_iter, __cur_iter) \
			(void)(BVEC_STEP);				      \
	}))

#endif 

struct ceph_bvec_iter {
	struct bio_vec *bvecs;
	struct bvec_iter iter;
};

#define __ceph_bvec_iter_advance_step(it, n, STEP) do {			      \
	BUG_ON((n) > (it)->iter.bi_size);				      \
	(void)(STEP);							      \
	bvec_iter_advance((it)->bvecs, &(it)->iter, (n));		      \
} while (0)


#define ceph_bvec_iter_advance(it, n)					      \
	__ceph_bvec_iter_advance_step(it, n, 0)


#define ceph_bvec_iter_advance_step(it, n, BVEC_STEP)			      \
	__ceph_bvec_iter_advance_step(it, n, ({				      \
		struct bio_vec bv;					      \
		struct bvec_iter __cur_iter;				      \
									      \
		__cur_iter = (it)->iter;				      \
		__cur_iter.bi_size = (n);				      \
		for_each_bvec(bv, (it)->bvecs, __cur_iter, __cur_iter)	      \
			(void)(BVEC_STEP);				      \
	}))

#define ceph_bvec_iter_shorten(it, n) do {				      \
	BUG_ON((n) > (it)->iter.bi_size);				      \
	(it)->iter.bi_size = (n);					      \
} while (0)

struct ceph_msg_data {
	enum ceph_msg_data_type		type;
	union {
#ifdef CONFIG_BLOCK
		struct {
			struct ceph_bio_iter	bio_pos;
			u32			bio_length;
		};
#endif 
		struct ceph_bvec_iter	bvec_pos;
		struct {
			struct page	**pages;
			size_t		length;		
			unsigned int	alignment;	
			bool		own_pages;
		};
		struct ceph_pagelist	*pagelist;
	};
};

struct ceph_msg_data_cursor {
	size_t			total_resid;	

	struct ceph_msg_data	*data;		
	size_t			resid;		
	bool			last_piece;	
	bool			need_crc;	
	union {
#ifdef CONFIG_BLOCK
		struct ceph_bio_iter	bio_iter;
#endif 
		struct bvec_iter	bvec_iter;
		struct {				
			unsigned int	page_offset;	
			unsigned short	page_index;	
			unsigned short	page_count;	
		};
		struct {				
			struct page	*page;		
			size_t		offset;		
		};
	};
};


struct ceph_msg {
	struct ceph_msg_header hdr;	
	union {
		struct ceph_msg_footer footer;		
		struct ceph_msg_footer_old old_footer;	
	};
	struct kvec front;              
	struct ceph_buffer *middle;

	size_t				data_length;
	struct ceph_msg_data		*data;
	int				num_data_items;
	int				max_data_items;
	struct ceph_msg_data_cursor	cursor;

	struct ceph_connection *con;
	struct list_head list_head;	

	struct kref kref;
	bool more_to_follow;
	bool needs_out_seq;
	int front_alloc_len;
	unsigned long ack_stamp;        

	struct ceph_msgpool *pool;
};


#define BASE_DELAY_INTERVAL	(HZ/2)
#define MAX_DELAY_INTERVAL	(5 * 60 * HZ)


struct ceph_connection {
	void *private;

	const struct ceph_connection_operations *ops;

	struct ceph_messenger *msgr;

	atomic_t sock_state;
	struct socket *sock;
	struct ceph_entity_addr peer_addr; 
	struct ceph_entity_addr peer_addr_for_me;

	unsigned long flags;
	unsigned long state;
	const char *error_msg;  

	struct ceph_entity_name peer_name; 

	u64 peer_features;
	u32 connect_seq;      
	u32 peer_global_seq;  

	struct ceph_auth_handshake *auth;
	int auth_retry;       

	struct mutex mutex;

	
	struct list_head out_queue;
	struct list_head out_sent;   
	u64 out_seq;		     

	u64 in_seq, in_seq_acked;  

	
	char in_banner[CEPH_BANNER_MAX_LEN];
	struct ceph_msg_connect out_connect;
	struct ceph_msg_connect_reply in_reply;
	struct ceph_entity_addr actual_peer_addr;

	
	struct ceph_msg_header out_hdr;
	struct ceph_msg *out_msg;        
	bool out_msg_done;

	struct kvec out_kvec[8],         
		*out_kvec_cur;
	int out_kvec_left;   
	int out_skip;        
	int out_kvec_bytes;  
	int out_more;        
	__le64 out_temp_ack; 
	struct ceph_timespec out_temp_keepalive2; 

	
	struct ceph_msg_header in_hdr;
	struct ceph_msg *in_msg;
	u32 in_front_crc, in_middle_crc, in_data_crc;  

	char in_tag;         
	int in_base_pos;     
	__le64 in_temp_ack;  

	struct timespec64 last_keepalive_ack; 

	struct delayed_work work;	    
	unsigned long       delay;          
};


extern const char *ceph_pr_addr(const struct ceph_entity_addr *addr);

extern int ceph_parse_ips(const char *c, const char *end,
			  struct ceph_entity_addr *addr,
			  int max_count, int *count);


extern int ceph_msgr_init(void);
extern void ceph_msgr_exit(void);
extern void ceph_msgr_flush(void);

extern void ceph_messenger_init(struct ceph_messenger *msgr,
				struct ceph_entity_addr *myaddr);
extern void ceph_messenger_fini(struct ceph_messenger *msgr);
extern void ceph_messenger_reset_nonce(struct ceph_messenger *msgr);

extern void ceph_con_init(struct ceph_connection *con, void *private,
			const struct ceph_connection_operations *ops,
			struct ceph_messenger *msgr);
extern void ceph_con_open(struct ceph_connection *con,
			  __u8 entity_type, __u64 entity_num,
			  struct ceph_entity_addr *addr);
extern bool ceph_con_opened(struct ceph_connection *con);
extern void ceph_con_close(struct ceph_connection *con);
extern void ceph_con_send(struct ceph_connection *con, struct ceph_msg *msg);

extern void ceph_msg_revoke(struct ceph_msg *msg);
extern void ceph_msg_revoke_incoming(struct ceph_msg *msg);

extern void ceph_con_keepalive(struct ceph_connection *con);
extern bool ceph_con_keepalive_expired(struct ceph_connection *con,
				       unsigned long interval);

void ceph_msg_data_add_pages(struct ceph_msg *msg, struct page **pages,
			     size_t length, size_t alignment, bool own_pages);
extern void ceph_msg_data_add_pagelist(struct ceph_msg *msg,
				struct ceph_pagelist *pagelist);
#ifdef CONFIG_BLOCK
void ceph_msg_data_add_bio(struct ceph_msg *msg, struct ceph_bio_iter *bio_pos,
			   u32 length);
#endif 
void ceph_msg_data_add_bvecs(struct ceph_msg *msg,
			     struct ceph_bvec_iter *bvec_pos);

struct ceph_msg *ceph_msg_new2(int type, int front_len, int max_data_items,
			       gfp_t flags, bool can_fail);
extern struct ceph_msg *ceph_msg_new(int type, int front_len, gfp_t flags,
				     bool can_fail);

extern struct ceph_msg *ceph_msg_get(struct ceph_msg *msg);
extern void ceph_msg_put(struct ceph_msg *msg);

extern void ceph_msg_dump(struct ceph_msg *msg);

#endif
