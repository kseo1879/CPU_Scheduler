/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _LINUX_ETHTOOL_H
#define _LINUX_ETHTOOL_H

#include <linux/bitmap.h>
#include <linux/compat.h>
#include <uapi/linux/ethtool.h>

#ifdef CONFIG_COMPAT

struct compat_ethtool_rx_flow_spec {
	u32		flow_type;
	union ethtool_flow_union h_u;
	struct ethtool_flow_ext h_ext;
	union ethtool_flow_union m_u;
	struct ethtool_flow_ext m_ext;
	compat_u64	ring_cookie;
	u32		location;
};

struct compat_ethtool_rxnfc {
	u32				cmd;
	u32				flow_type;
	compat_u64			data;
	struct compat_ethtool_rx_flow_spec fs;
	u32				rule_cnt;
	u32				rule_locs[];
};

#endif 

#include <linux/rculist.h>


enum ethtool_phys_id_state {
	ETHTOOL_ID_INACTIVE,
	ETHTOOL_ID_ACTIVE,
	ETHTOOL_ID_ON,
	ETHTOOL_ID_OFF
};

enum {
	ETH_RSS_HASH_TOP_BIT, 
	ETH_RSS_HASH_XOR_BIT, 
	ETH_RSS_HASH_CRC32_BIT, 

	
	ETH_RSS_HASH_FUNCS_COUNT
};

#define __ETH_RSS_HASH_BIT(bit)	((u32)1 << (bit))
#define __ETH_RSS_HASH(name)	__ETH_RSS_HASH_BIT(ETH_RSS_HASH_##name##_BIT)

#define ETH_RSS_HASH_TOP	__ETH_RSS_HASH(TOP)
#define ETH_RSS_HASH_XOR	__ETH_RSS_HASH(XOR)
#define ETH_RSS_HASH_CRC32	__ETH_RSS_HASH(CRC32)

#define ETH_RSS_HASH_UNKNOWN	0
#define ETH_RSS_HASH_NO_CHANGE	0

struct net_device;


u32 ethtool_op_get_link(struct net_device *dev);
int ethtool_op_get_ts_info(struct net_device *dev, struct ethtool_ts_info *eti);



struct ethtool_link_ext_state_info {
	enum ethtool_link_ext_state link_ext_state;
	union {
		enum ethtool_link_ext_substate_autoneg autoneg;
		enum ethtool_link_ext_substate_link_training link_training;
		enum ethtool_link_ext_substate_link_logical_mismatch link_logical_mismatch;
		enum ethtool_link_ext_substate_bad_signal_integrity bad_signal_integrity;
		enum ethtool_link_ext_substate_cable_issue cable_issue;
		u8 __link_ext_substate;
	};
};


static inline u32 ethtool_rxfh_indir_default(u32 index, u32 n_rx_rings)
{
	return index % n_rx_rings;
}


#define __ETHTOOL_DECLARE_LINK_MODE_MASK(name)		\
	DECLARE_BITMAP(name, __ETHTOOL_LINK_MODE_MASK_NBITS)


struct ethtool_link_ksettings {
	struct ethtool_link_settings base;
	struct {
		__ETHTOOL_DECLARE_LINK_MODE_MASK(supported);
		__ETHTOOL_DECLARE_LINK_MODE_MASK(advertising);
		__ETHTOOL_DECLARE_LINK_MODE_MASK(lp_advertising);
	} link_modes;
};


#define ethtool_link_ksettings_zero_link_mode(ptr, name)		\
	bitmap_zero((ptr)->link_modes.name, __ETHTOOL_LINK_MODE_MASK_NBITS)


#define ethtool_link_ksettings_add_link_mode(ptr, name, mode)		\
	__set_bit(ETHTOOL_LINK_MODE_ ## mode ## _BIT, (ptr)->link_modes.name)


#define ethtool_link_ksettings_del_link_mode(ptr, name, mode)		\
	__clear_bit(ETHTOOL_LINK_MODE_ ## mode ## _BIT, (ptr)->link_modes.name)


#define ethtool_link_ksettings_test_link_mode(ptr, name, mode)		\
	test_bit(ETHTOOL_LINK_MODE_ ## mode ## _BIT, (ptr)->link_modes.name)

extern int
__ethtool_get_link_ksettings(struct net_device *dev,
			     struct ethtool_link_ksettings *link_ksettings);


void ethtool_intersect_link_masks(struct ethtool_link_ksettings *dst,
				  struct ethtool_link_ksettings *src);

void ethtool_convert_legacy_u32_to_link_mode(unsigned long *dst,
					     u32 legacy_u32);


bool ethtool_convert_link_mode_to_legacy_u32(u32 *legacy_u32,
				     const unsigned long *src);

#define ETHTOOL_COALESCE_RX_USECS		BIT(0)
#define ETHTOOL_COALESCE_RX_MAX_FRAMES		BIT(1)
#define ETHTOOL_COALESCE_RX_USECS_IRQ		BIT(2)
#define ETHTOOL_COALESCE_RX_MAX_FRAMES_IRQ	BIT(3)
#define ETHTOOL_COALESCE_TX_USECS		BIT(4)
#define ETHTOOL_COALESCE_TX_MAX_FRAMES		BIT(5)
#define ETHTOOL_COALESCE_TX_USECS_IRQ		BIT(6)
#define ETHTOOL_COALESCE_TX_MAX_FRAMES_IRQ	BIT(7)
#define ETHTOOL_COALESCE_STATS_BLOCK_USECS	BIT(8)
#define ETHTOOL_COALESCE_USE_ADAPTIVE_RX	BIT(9)
#define ETHTOOL_COALESCE_USE_ADAPTIVE_TX	BIT(10)
#define ETHTOOL_COALESCE_PKT_RATE_LOW		BIT(11)
#define ETHTOOL_COALESCE_RX_USECS_LOW		BIT(12)
#define ETHTOOL_COALESCE_RX_MAX_FRAMES_LOW	BIT(13)
#define ETHTOOL_COALESCE_TX_USECS_LOW		BIT(14)
#define ETHTOOL_COALESCE_TX_MAX_FRAMES_LOW	BIT(15)
#define ETHTOOL_COALESCE_PKT_RATE_HIGH		BIT(16)
#define ETHTOOL_COALESCE_RX_USECS_HIGH		BIT(17)
#define ETHTOOL_COALESCE_RX_MAX_FRAMES_HIGH	BIT(18)
#define ETHTOOL_COALESCE_TX_USECS_HIGH		BIT(19)
#define ETHTOOL_COALESCE_TX_MAX_FRAMES_HIGH	BIT(20)
#define ETHTOOL_COALESCE_RATE_SAMPLE_INTERVAL	BIT(21)

#define ETHTOOL_COALESCE_USECS						\
	(ETHTOOL_COALESCE_RX_USECS | ETHTOOL_COALESCE_TX_USECS)
#define ETHTOOL_COALESCE_MAX_FRAMES					\
	(ETHTOOL_COALESCE_RX_MAX_FRAMES | ETHTOOL_COALESCE_TX_MAX_FRAMES)
#define ETHTOOL_COALESCE_USECS_IRQ					\
	(ETHTOOL_COALESCE_RX_USECS_IRQ | ETHTOOL_COALESCE_TX_USECS_IRQ)
#define ETHTOOL_COALESCE_MAX_FRAMES_IRQ		\
	(ETHTOOL_COALESCE_RX_MAX_FRAMES_IRQ |	\
	 ETHTOOL_COALESCE_TX_MAX_FRAMES_IRQ)
#define ETHTOOL_COALESCE_USE_ADAPTIVE					\
	(ETHTOOL_COALESCE_USE_ADAPTIVE_RX | ETHTOOL_COALESCE_USE_ADAPTIVE_TX)
#define ETHTOOL_COALESCE_USECS_LOW_HIGH					\
	(ETHTOOL_COALESCE_RX_USECS_LOW | ETHTOOL_COALESCE_TX_USECS_LOW | \
	 ETHTOOL_COALESCE_RX_USECS_HIGH | ETHTOOL_COALESCE_TX_USECS_HIGH)
#define ETHTOOL_COALESCE_MAX_FRAMES_LOW_HIGH	\
	(ETHTOOL_COALESCE_RX_MAX_FRAMES_LOW |	\
	 ETHTOOL_COALESCE_TX_MAX_FRAMES_LOW |	\
	 ETHTOOL_COALESCE_RX_MAX_FRAMES_HIGH |	\
	 ETHTOOL_COALESCE_TX_MAX_FRAMES_HIGH)
#define ETHTOOL_COALESCE_PKT_RATE_RX_USECS				\
	(ETHTOOL_COALESCE_USE_ADAPTIVE_RX |				\
	 ETHTOOL_COALESCE_RX_USECS_LOW | ETHTOOL_COALESCE_RX_USECS_HIGH | \
	 ETHTOOL_COALESCE_PKT_RATE_LOW | ETHTOOL_COALESCE_PKT_RATE_HIGH | \
	 ETHTOOL_COALESCE_RATE_SAMPLE_INTERVAL)

#define ETHTOOL_STAT_NOT_SET	(~0ULL)


struct ethtool_pause_stats {
	u64 tx_pause_frames;
	u64 rx_pause_frames;
};


struct ethtool_ops {
	u32	supported_coalesce_params;
	void	(*get_drvinfo)(struct net_device *, struct ethtool_drvinfo *);
	int	(*get_regs_len)(struct net_device *);
	void	(*get_regs)(struct net_device *, struct ethtool_regs *, void *);
	void	(*get_wol)(struct net_device *, struct ethtool_wolinfo *);
	int	(*set_wol)(struct net_device *, struct ethtool_wolinfo *);
	u32	(*get_msglevel)(struct net_device *);
	void	(*set_msglevel)(struct net_device *, u32);
	int	(*nway_reset)(struct net_device *);
	u32	(*get_link)(struct net_device *);
	int	(*get_link_ext_state)(struct net_device *,
				      struct ethtool_link_ext_state_info *);
	int	(*get_eeprom_len)(struct net_device *);
	int	(*get_eeprom)(struct net_device *,
			      struct ethtool_eeprom *, u8 *);
	int	(*set_eeprom)(struct net_device *,
			      struct ethtool_eeprom *, u8 *);
	int	(*get_coalesce)(struct net_device *, struct ethtool_coalesce *);
	int	(*set_coalesce)(struct net_device *, struct ethtool_coalesce *);
	void	(*get_ringparam)(struct net_device *,
				 struct ethtool_ringparam *);
	int	(*set_ringparam)(struct net_device *,
				 struct ethtool_ringparam *);
	void	(*get_pause_stats)(struct net_device *dev,
				   struct ethtool_pause_stats *pause_stats);
	void	(*get_pauseparam)(struct net_device *,
				  struct ethtool_pauseparam*);
	int	(*set_pauseparam)(struct net_device *,
				  struct ethtool_pauseparam*);
	void	(*self_test)(struct net_device *, struct ethtool_test *, u64 *);
	void	(*get_strings)(struct net_device *, u32 stringset, u8 *);
	int	(*set_phys_id)(struct net_device *, enum ethtool_phys_id_state);
	void	(*get_ethtool_stats)(struct net_device *,
				     struct ethtool_stats *, u64 *);
	int	(*begin)(struct net_device *);
	void	(*complete)(struct net_device *);
	u32	(*get_priv_flags)(struct net_device *);
	int	(*set_priv_flags)(struct net_device *, u32);
	int	(*get_sset_count)(struct net_device *, int);
	int	(*get_rxnfc)(struct net_device *,
			     struct ethtool_rxnfc *, u32 *rule_locs);
	int	(*set_rxnfc)(struct net_device *, struct ethtool_rxnfc *);
	int	(*flash_device)(struct net_device *, struct ethtool_flash *);
	int	(*reset)(struct net_device *, u32 *);
	u32	(*get_rxfh_key_size)(struct net_device *);
	u32	(*get_rxfh_indir_size)(struct net_device *);
	int	(*get_rxfh)(struct net_device *, u32 *indir, u8 *key,
			    u8 *hfunc);
	int	(*set_rxfh)(struct net_device *, const u32 *indir,
			    const u8 *key, const u8 hfunc);
	int	(*get_rxfh_context)(struct net_device *, u32 *indir, u8 *key,
				    u8 *hfunc, u32 rss_context);
	int	(*set_rxfh_context)(struct net_device *, const u32 *indir,
				    const u8 *key, const u8 hfunc,
				    u32 *rss_context, bool delete);
	void	(*get_channels)(struct net_device *, struct ethtool_channels *);
	int	(*set_channels)(struct net_device *, struct ethtool_channels *);
	int	(*get_dump_flag)(struct net_device *, struct ethtool_dump *);
	int	(*get_dump_data)(struct net_device *,
				 struct ethtool_dump *, void *);
	int	(*set_dump)(struct net_device *, struct ethtool_dump *);
	int	(*get_ts_info)(struct net_device *, struct ethtool_ts_info *);
	int     (*get_module_info)(struct net_device *,
				   struct ethtool_modinfo *);
	int     (*get_module_eeprom)(struct net_device *,
				     struct ethtool_eeprom *, u8 *);
	int	(*get_eee)(struct net_device *, struct ethtool_eee *);
	int	(*set_eee)(struct net_device *, struct ethtool_eee *);
	int	(*get_tunable)(struct net_device *,
			       const struct ethtool_tunable *, void *);
	int	(*set_tunable)(struct net_device *,
			       const struct ethtool_tunable *, const void *);
	int	(*get_per_queue_coalesce)(struct net_device *, u32,
					  struct ethtool_coalesce *);
	int	(*set_per_queue_coalesce)(struct net_device *, u32,
					  struct ethtool_coalesce *);
	int	(*get_link_ksettings)(struct net_device *,
				      struct ethtool_link_ksettings *);
	int	(*set_link_ksettings)(struct net_device *,
				      const struct ethtool_link_ksettings *);
	int	(*get_fecparam)(struct net_device *,
				      struct ethtool_fecparam *);
	int	(*set_fecparam)(struct net_device *,
				      struct ethtool_fecparam *);
	void	(*get_ethtool_phy_stats)(struct net_device *,
					 struct ethtool_stats *, u64 *);
	int	(*get_phy_tunable)(struct net_device *,
				   const struct ethtool_tunable *, void *);
	int	(*set_phy_tunable)(struct net_device *,
				   const struct ethtool_tunable *, const void *);
};

int ethtool_check_ops(const struct ethtool_ops *ops);

struct ethtool_rx_flow_rule {
	struct flow_rule	*rule;
	unsigned long		priv[];
};

struct ethtool_rx_flow_spec_input {
	const struct ethtool_rx_flow_spec	*fs;
	u32					rss_ctx;
};

struct ethtool_rx_flow_rule *
ethtool_rx_flow_rule_create(const struct ethtool_rx_flow_spec_input *input);
void ethtool_rx_flow_rule_destroy(struct ethtool_rx_flow_rule *rule);

bool ethtool_virtdev_validate_cmd(const struct ethtool_link_ksettings *cmd);
int ethtool_virtdev_set_link_ksettings(struct net_device *dev,
				       const struct ethtool_link_ksettings *cmd,
				       u32 *dev_speed, u8 *dev_duplex);

struct netlink_ext_ack;
struct phy_device;
struct phy_tdr_config;


struct ethtool_phy_ops {
	int (*get_sset_count)(struct phy_device *dev);
	int (*get_strings)(struct phy_device *dev, u8 *data);
	int (*get_stats)(struct phy_device *dev,
			 struct ethtool_stats *stats, u64 *data);
	int (*start_cable_test)(struct phy_device *phydev,
				struct netlink_ext_ack *extack);
	int (*start_cable_test_tdr)(struct phy_device *phydev,
				    struct netlink_ext_ack *extack,
				    const struct phy_tdr_config *config);
};


void ethtool_set_ethtool_phy_ops(const struct ethtool_phy_ops *ops);

#endif 
