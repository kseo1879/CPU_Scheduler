#ifndef NETDEV_PCS_H
#define NETDEV_PCS_H

#include <linux/phy.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>

struct device_node;
struct ethtool_cmd;
struct fwnode_handle;
struct net_device;

enum {
	MLO_PAUSE_NONE,
	MLO_PAUSE_RX = BIT(0),
	MLO_PAUSE_TX = BIT(1),
	MLO_PAUSE_TXRX_MASK = MLO_PAUSE_TX | MLO_PAUSE_RX,
	MLO_PAUSE_AN = BIT(2),

	MLO_AN_PHY = 0,	
	MLO_AN_FIXED,	
	MLO_AN_INBAND,	
};

static inline bool phylink_autoneg_inband(unsigned int mode)
{
	return mode == MLO_AN_INBAND;
}


struct phylink_link_state {
	__ETHTOOL_DECLARE_LINK_MODE_MASK(advertising);
	__ETHTOOL_DECLARE_LINK_MODE_MASK(lp_advertising);
	phy_interface_t interface;
	int speed;
	int duplex;
	int pause;
	unsigned int link:1;
	unsigned int an_enabled:1;
	unsigned int an_complete:1;
};

enum phylink_op_type {
	PHYLINK_NETDEV = 0,
	PHYLINK_DEV,
};


struct phylink_config {
	struct device *dev;
	enum phylink_op_type type;
	bool pcs_poll;
	bool poll_fixed_state;
	void (*get_fixed_state)(struct phylink_config *config,
				struct phylink_link_state *state);
};


struct phylink_mac_ops {
	void (*validate)(struct phylink_config *config,
			 unsigned long *supported,
			 struct phylink_link_state *state);
	void (*mac_pcs_get_state)(struct phylink_config *config,
				  struct phylink_link_state *state);
	int (*mac_prepare)(struct phylink_config *config, unsigned int mode,
			   phy_interface_t iface);
	void (*mac_config)(struct phylink_config *config, unsigned int mode,
			   const struct phylink_link_state *state);
	int (*mac_finish)(struct phylink_config *config, unsigned int mode,
			  phy_interface_t iface);
	void (*mac_an_restart)(struct phylink_config *config);
	void (*mac_link_down)(struct phylink_config *config, unsigned int mode,
			      phy_interface_t interface);
	void (*mac_link_up)(struct phylink_config *config,
			    struct phy_device *phy, unsigned int mode,
			    phy_interface_t interface, int speed, int duplex,
			    bool tx_pause, bool rx_pause);
};

#if 0 

void validate(struct phylink_config *config, unsigned long *supported,
	      struct phylink_link_state *state);


void mac_pcs_get_state(struct phylink_config *config,
		       struct phylink_link_state *state);


int mac_prepare(struct phylink_config *config, unsigned int mode,
		phy_interface_t iface);


void mac_config(struct phylink_config *config, unsigned int mode,
		const struct phylink_link_state *state);


int mac_finish(struct phylink_config *config, unsigned int mode,
		phy_interface_t iface);


void mac_an_restart(struct phylink_config *config);


void mac_link_down(struct phylink_config *config, unsigned int mode,
		   phy_interface_t interface);


void mac_link_up(struct phylink_config *config, struct phy_device *phy,
		 unsigned int mode, phy_interface_t interface,
		 int speed, int duplex, bool tx_pause, bool rx_pause);
#endif

struct phylink_pcs_ops;


struct phylink_pcs {
	const struct phylink_pcs_ops *ops;
	bool poll;
};


struct phylink_pcs_ops {
	void (*pcs_get_state)(struct phylink_pcs *pcs,
			      struct phylink_link_state *state);
	int (*pcs_config)(struct phylink_pcs *pcs, unsigned int mode,
			  phy_interface_t interface,
			  const unsigned long *advertising,
			  bool permit_pause_to_mac);
	void (*pcs_an_restart)(struct phylink_pcs *pcs);
	void (*pcs_link_up)(struct phylink_pcs *pcs, unsigned int mode,
			    phy_interface_t interface, int speed, int duplex);
};

#if 0 

void pcs_get_state(struct phylink_pcs *pcs,
		   struct phylink_link_state *state);


int pcs_config(struct phylink_pcs *pcs, unsigned int mode,
	       phy_interface_t interface, const unsigned long *advertising,
	       bool permit_pause_to_mac);


void pcs_an_restart(struct phylink_pcs *pcs);


void pcs_link_up(struct phylink_pcs *pcs, unsigned int mode,
		 phy_interface_t interface, int speed, int duplex);
#endif

struct phylink *phylink_create(struct phylink_config *, struct fwnode_handle *,
			       phy_interface_t iface,
			       const struct phylink_mac_ops *mac_ops);
void phylink_set_pcs(struct phylink *, struct phylink_pcs *pcs);
void phylink_destroy(struct phylink *);

int phylink_connect_phy(struct phylink *, struct phy_device *);
int phylink_of_phy_connect(struct phylink *, struct device_node *, u32 flags);
void phylink_disconnect_phy(struct phylink *);

void phylink_mac_change(struct phylink *, bool up);

void phylink_start(struct phylink *);
void phylink_stop(struct phylink *);

void phylink_ethtool_get_wol(struct phylink *, struct ethtool_wolinfo *);
int phylink_ethtool_set_wol(struct phylink *, struct ethtool_wolinfo *);

int phylink_ethtool_ksettings_get(struct phylink *,
				  struct ethtool_link_ksettings *);
int phylink_ethtool_ksettings_set(struct phylink *,
				  const struct ethtool_link_ksettings *);
int phylink_ethtool_nway_reset(struct phylink *);
void phylink_ethtool_get_pauseparam(struct phylink *,
				    struct ethtool_pauseparam *);
int phylink_ethtool_set_pauseparam(struct phylink *,
				   struct ethtool_pauseparam *);
int phylink_get_eee_err(struct phylink *);
int phylink_init_eee(struct phylink *, bool);
int phylink_ethtool_get_eee(struct phylink *, struct ethtool_eee *);
int phylink_ethtool_set_eee(struct phylink *, struct ethtool_eee *);
int phylink_mii_ioctl(struct phylink *, struct ifreq *, int);
int phylink_speed_down(struct phylink *pl, bool sync);
int phylink_speed_up(struct phylink *pl);

#define phylink_zero(bm) \
	bitmap_zero(bm, __ETHTOOL_LINK_MODE_MASK_NBITS)
#define __phylink_do_bit(op, bm, mode) \
	op(ETHTOOL_LINK_MODE_ ## mode ## _BIT, bm)

#define phylink_set(bm, mode)	__phylink_do_bit(__set_bit, bm, mode)
#define phylink_clear(bm, mode)	__phylink_do_bit(__clear_bit, bm, mode)
#define phylink_test(bm, mode)	__phylink_do_bit(test_bit, bm, mode)

void phylink_set_port_modes(unsigned long *bits);
void phylink_helper_basex_speed(struct phylink_link_state *state);

void phylink_mii_c22_pcs_get_state(struct mdio_device *pcs,
				   struct phylink_link_state *state);
int phylink_mii_c22_pcs_set_advertisement(struct mdio_device *pcs,
					  phy_interface_t interface,
					  const unsigned long *advertising);
int phylink_mii_c22_pcs_config(struct mdio_device *pcs, unsigned int mode,
			       phy_interface_t interface,
			       const unsigned long *advertising);
void phylink_mii_c22_pcs_an_restart(struct mdio_device *pcs);

void phylink_mii_c45_pcs_get_state(struct mdio_device *pcs,
				   struct phylink_link_state *state);

void phylink_decode_usxgmii_word(struct phylink_link_state *state,
				 uint16_t lpa);
#endif
