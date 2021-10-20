/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __LINUX_USB_TYPEC_H
#define __LINUX_USB_TYPEC_H

#include <linux/types.h>


#define USB_TYPEC_REV_1_0	0x100 
#define USB_TYPEC_REV_1_1	0x110 
#define USB_TYPEC_REV_1_2	0x120 
#define USB_TYPEC_REV_1_3	0x130 
#define USB_TYPEC_REV_1_4	0x140 
#define USB_TYPEC_REV_2_0	0x200 

struct typec_partner;
struct typec_cable;
struct typec_plug;
struct typec_port;

struct fwnode_handle;
struct device;

enum typec_port_type {
	TYPEC_PORT_SRC,
	TYPEC_PORT_SNK,
	TYPEC_PORT_DRP,
};

enum typec_port_data {
	TYPEC_PORT_DFP,
	TYPEC_PORT_UFP,
	TYPEC_PORT_DRD,
};

enum typec_plug_type {
	USB_PLUG_NONE,
	USB_PLUG_TYPE_A,
	USB_PLUG_TYPE_B,
	USB_PLUG_TYPE_C,
	USB_PLUG_CAPTIVE,
};

enum typec_data_role {
	TYPEC_DEVICE,
	TYPEC_HOST,
};

enum typec_role {
	TYPEC_SINK,
	TYPEC_SOURCE,
};

enum typec_pwr_opmode {
	TYPEC_PWR_MODE_USB,
	TYPEC_PWR_MODE_1_5A,
	TYPEC_PWR_MODE_3_0A,
	TYPEC_PWR_MODE_PD,
};

enum typec_accessory {
	TYPEC_ACCESSORY_NONE,
	TYPEC_ACCESSORY_AUDIO,
	TYPEC_ACCESSORY_DEBUG,
};

#define TYPEC_MAX_ACCESSORY	3

enum typec_orientation {
	TYPEC_ORIENTATION_NONE,
	TYPEC_ORIENTATION_NORMAL,
	TYPEC_ORIENTATION_REVERSE,
};


struct enter_usb_data {
	u32			eudo;
	unsigned char		active_link_training:1;
};


struct usb_pd_identity {
	u32			id_header;
	u32			cert_stat;
	u32			product;
	u32			vdo[3];
};

int typec_partner_set_identity(struct typec_partner *partner);
int typec_cable_set_identity(struct typec_cable *cable);


struct typec_altmode_desc {
	u16			svid;
	u8			mode;
	u32			vdo;
	
	enum typec_port_data	roles;
};

struct typec_altmode
*typec_partner_register_altmode(struct typec_partner *partner,
				const struct typec_altmode_desc *desc);
struct typec_altmode
*typec_plug_register_altmode(struct typec_plug *plug,
			     const struct typec_altmode_desc *desc);
struct typec_altmode
*typec_port_register_altmode(struct typec_port *port,
			     const struct typec_altmode_desc *desc);
void typec_unregister_altmode(struct typec_altmode *altmode);

struct typec_port *typec_altmode2port(struct typec_altmode *alt);

void typec_altmode_update_active(struct typec_altmode *alt, bool active);

enum typec_plug_index {
	TYPEC_PLUG_SOP_P,
	TYPEC_PLUG_SOP_PP,
};


struct typec_plug_desc {
	enum typec_plug_index	index;
};


struct typec_cable_desc {
	enum typec_plug_type	type;
	unsigned int		active:1;
	struct usb_pd_identity	*identity;
};


struct typec_partner_desc {
	unsigned int		usb_pd:1;
	enum typec_accessory	accessory;
	struct usb_pd_identity	*identity;
};


struct typec_operations {
	int (*try_role)(struct typec_port *port, int role);
	int (*dr_set)(struct typec_port *port, enum typec_data_role role);
	int (*pr_set)(struct typec_port *port, enum typec_role role);
	int (*vconn_set)(struct typec_port *port, enum typec_role role);
	int (*port_type_set)(struct typec_port *port,
			     enum typec_port_type type);
};


struct typec_capability {
	enum typec_port_type	type;
	enum typec_port_data	data;
	u16			revision; 
	u16			pd_revision; 
	int			prefer_role;
	enum typec_accessory	accessory[TYPEC_MAX_ACCESSORY];
	unsigned int		orientation_aware:1;

	struct fwnode_handle	*fwnode;
	void			*driver_data;

	const struct typec_operations	*ops;
};


#define TYPEC_NO_PREFERRED_ROLE	(-1)

struct typec_port *typec_register_port(struct device *parent,
				       const struct typec_capability *cap);
void typec_unregister_port(struct typec_port *port);

struct typec_partner *typec_register_partner(struct typec_port *port,
					     struct typec_partner_desc *desc);
void typec_unregister_partner(struct typec_partner *partner);

struct typec_cable *typec_register_cable(struct typec_port *port,
					 struct typec_cable_desc *desc);
void typec_unregister_cable(struct typec_cable *cable);

struct typec_cable *typec_cable_get(struct typec_port *port);
void typec_cable_put(struct typec_cable *cable);
int typec_cable_is_active(struct typec_cable *cable);

struct typec_plug *typec_register_plug(struct typec_cable *cable,
				       struct typec_plug_desc *desc);
void typec_unregister_plug(struct typec_plug *plug);

void typec_set_data_role(struct typec_port *port, enum typec_data_role role);
void typec_set_pwr_role(struct typec_port *port, enum typec_role role);
void typec_set_vconn_role(struct typec_port *port, enum typec_role role);
void typec_set_pwr_opmode(struct typec_port *port, enum typec_pwr_opmode mode);

int typec_set_orientation(struct typec_port *port,
			  enum typec_orientation orientation);
enum typec_orientation typec_get_orientation(struct typec_port *port);
int typec_set_mode(struct typec_port *port, int mode);

void *typec_get_drvdata(struct typec_port *port);

int typec_find_pwr_opmode(const char *name);
int typec_find_orientation(const char *name);
int typec_find_port_power_role(const char *name);
int typec_find_power_role(const char *name);
int typec_find_port_data_role(const char *name);
#endif 
