/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _NET_FLOW_DISSECTOR_H
#define _NET_FLOW_DISSECTOR_H

#include <linux/types.h>
#include <linux/in6.h>
#include <linux/siphash.h>
#include <linux/string.h>
#include <uapi/linux/if_ether.h>

struct bpf_prog;
struct net;
struct sk_buff;


struct flow_dissector_key_control {
	u16	thoff;
	u16	addr_type;
	u32	flags;
};

#define FLOW_DIS_IS_FRAGMENT	BIT(0)
#define FLOW_DIS_FIRST_FRAG	BIT(1)
#define FLOW_DIS_ENCAPSULATION	BIT(2)

enum flow_dissect_ret {
	FLOW_DISSECT_RET_OUT_GOOD,
	FLOW_DISSECT_RET_OUT_BAD,
	FLOW_DISSECT_RET_PROTO_AGAIN,
	FLOW_DISSECT_RET_IPPROTO_AGAIN,
	FLOW_DISSECT_RET_CONTINUE,
};


struct flow_dissector_key_basic {
	__be16	n_proto;
	u8	ip_proto;
	u8	padding;
};

struct flow_dissector_key_tags {
	u32	flow_label;
};

struct flow_dissector_key_vlan {
	union {
		struct {
			u16	vlan_id:12,
				vlan_dei:1,
				vlan_priority:3;
		};
		__be16	vlan_tci;
	};
	__be16	vlan_tpid;
};

struct flow_dissector_mpls_lse {
	u32	mpls_ttl:8,
		mpls_bos:1,
		mpls_tc:3,
		mpls_label:20;
};

#define FLOW_DIS_MPLS_MAX 7
struct flow_dissector_key_mpls {
	struct flow_dissector_mpls_lse ls[FLOW_DIS_MPLS_MAX]; 
	u8 used_lses; 
};

static inline void dissector_set_mpls_lse(struct flow_dissector_key_mpls *mpls,
					  int lse_index)
{
	mpls->used_lses |= 1 << lse_index;
}

#define FLOW_DIS_TUN_OPTS_MAX 255

struct flow_dissector_key_enc_opts {
	u8 data[FLOW_DIS_TUN_OPTS_MAX];	
	u8 len;
	__be16 dst_opt_type;
};

struct flow_dissector_key_keyid {
	__be32	keyid;
};


struct flow_dissector_key_ipv4_addrs {
	
	__be32 src;
	__be32 dst;
};


struct flow_dissector_key_ipv6_addrs {
	
	struct in6_addr src;
	struct in6_addr dst;
};


struct flow_dissector_key_tipc {
	__be32 key;
};


struct flow_dissector_key_addrs {
	union {
		struct flow_dissector_key_ipv4_addrs v4addrs;
		struct flow_dissector_key_ipv6_addrs v6addrs;
		struct flow_dissector_key_tipc tipckey;
	};
};


struct flow_dissector_key_arp {
	__u32 sip;
	__u32 tip;
	__u8 op;
	unsigned char sha[ETH_ALEN];
	unsigned char tha[ETH_ALEN];
};


struct flow_dissector_key_ports {
	union {
		__be32 ports;
		struct {
			__be16 src;
			__be16 dst;
		};
	};
};


struct flow_dissector_key_icmp {
	struct {
		u8 type;
		u8 code;
	};
	u16 id;
};


struct flow_dissector_key_eth_addrs {
	
	unsigned char dst[ETH_ALEN];
	unsigned char src[ETH_ALEN];
};


struct flow_dissector_key_tcp {
	__be16 flags;
};


struct flow_dissector_key_ip {
	__u8	tos;
	__u8	ttl;
};


struct flow_dissector_key_meta {
	int ingress_ifindex;
	u16 ingress_iftype;
};


struct flow_dissector_key_ct {
	u16	ct_state;
	u16	ct_zone;
	u32	ct_mark;
	u32	ct_labels[4];
};


struct flow_dissector_key_hash {
	u32 hash;
};

enum flow_dissector_key_id {
	FLOW_DISSECTOR_KEY_CONTROL, 
	FLOW_DISSECTOR_KEY_BASIC, 
	FLOW_DISSECTOR_KEY_IPV4_ADDRS, 
	FLOW_DISSECTOR_KEY_IPV6_ADDRS, 
	FLOW_DISSECTOR_KEY_PORTS, 
	FLOW_DISSECTOR_KEY_PORTS_RANGE, 
	FLOW_DISSECTOR_KEY_ICMP, 
	FLOW_DISSECTOR_KEY_ETH_ADDRS, 
	FLOW_DISSECTOR_KEY_TIPC, 
	FLOW_DISSECTOR_KEY_ARP, 
	FLOW_DISSECTOR_KEY_VLAN, 
	FLOW_DISSECTOR_KEY_FLOW_LABEL, 
	FLOW_DISSECTOR_KEY_GRE_KEYID, 
	FLOW_DISSECTOR_KEY_MPLS_ENTROPY, 
	FLOW_DISSECTOR_KEY_ENC_KEYID, 
	FLOW_DISSECTOR_KEY_ENC_IPV4_ADDRS, 
	FLOW_DISSECTOR_KEY_ENC_IPV6_ADDRS, 
	FLOW_DISSECTOR_KEY_ENC_CONTROL, 
	FLOW_DISSECTOR_KEY_ENC_PORTS, 
	FLOW_DISSECTOR_KEY_MPLS, 
	FLOW_DISSECTOR_KEY_TCP, 
	FLOW_DISSECTOR_KEY_IP, 
	FLOW_DISSECTOR_KEY_CVLAN, 
	FLOW_DISSECTOR_KEY_ENC_IP, 
	FLOW_DISSECTOR_KEY_ENC_OPTS, 
	FLOW_DISSECTOR_KEY_META, 
	FLOW_DISSECTOR_KEY_CT, 
	FLOW_DISSECTOR_KEY_HASH, 

	FLOW_DISSECTOR_KEY_MAX,
};

#define FLOW_DISSECTOR_F_PARSE_1ST_FRAG		BIT(0)
#define FLOW_DISSECTOR_F_STOP_AT_FLOW_LABEL	BIT(1)
#define FLOW_DISSECTOR_F_STOP_AT_ENCAP		BIT(2)

struct flow_dissector_key {
	enum flow_dissector_key_id key_id;
	size_t offset; 
};

struct flow_dissector {
	unsigned int used_keys; 
	unsigned short int offset[FLOW_DISSECTOR_KEY_MAX];
};

struct flow_keys_basic {
	struct flow_dissector_key_control control;
	struct flow_dissector_key_basic basic;
};

struct flow_keys {
	struct flow_dissector_key_control control;
#define FLOW_KEYS_HASH_START_FIELD basic
	struct flow_dissector_key_basic basic __aligned(SIPHASH_ALIGNMENT);
	struct flow_dissector_key_tags tags;
	struct flow_dissector_key_vlan vlan;
	struct flow_dissector_key_vlan cvlan;
	struct flow_dissector_key_keyid keyid;
	struct flow_dissector_key_ports ports;
	struct flow_dissector_key_icmp icmp;
	
	struct flow_dissector_key_addrs addrs;
};

#define FLOW_KEYS_HASH_OFFSET		\
	offsetof(struct flow_keys, FLOW_KEYS_HASH_START_FIELD)

__be32 flow_get_u32_src(const struct flow_keys *flow);
__be32 flow_get_u32_dst(const struct flow_keys *flow);

extern struct flow_dissector flow_keys_dissector;
extern struct flow_dissector flow_keys_basic_dissector;


#define FLOW_KEYS_DIGEST_LEN	16
struct flow_keys_digest {
	u8	data[FLOW_KEYS_DIGEST_LEN];
};

void make_flow_keys_digest(struct flow_keys_digest *digest,
			   const struct flow_keys *flow);

static inline bool flow_keys_have_l4(const struct flow_keys *keys)
{
	return (keys->ports.ports || keys->tags.flow_label);
}

u32 flow_hash_from_keys(struct flow_keys *keys);
void skb_flow_get_icmp_tci(const struct sk_buff *skb,
			   struct flow_dissector_key_icmp *key_icmp,
			   void *data, int thoff, int hlen);

static inline bool dissector_uses_key(const struct flow_dissector *flow_dissector,
				      enum flow_dissector_key_id key_id)
{
	return flow_dissector->used_keys & (1 << key_id);
}

static inline void *skb_flow_dissector_target(struct flow_dissector *flow_dissector,
					      enum flow_dissector_key_id key_id,
					      void *target_container)
{
	return ((char *)target_container) + flow_dissector->offset[key_id];
}

struct bpf_flow_dissector {
	struct bpf_flow_keys	*flow_keys;
	const struct sk_buff	*skb;
	void			*data;
	void			*data_end;
};

static inline void
flow_dissector_init_keys(struct flow_dissector_key_control *key_control,
			 struct flow_dissector_key_basic *key_basic)
{
	memset(key_control, 0, sizeof(*key_control));
	memset(key_basic, 0, sizeof(*key_basic));
}

#ifdef CONFIG_BPF_SYSCALL
int flow_dissector_bpf_prog_attach_check(struct net *net,
					 struct bpf_prog *prog);
#endif 

#endif
