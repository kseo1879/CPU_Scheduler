/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __USB_TYPEC_TBT_H
#define __USB_TYPEC_TBT_H

#include <linux/usb/typec_altmode.h>

#define USB_TYPEC_VENDOR_INTEL		0x8087

#define USB_TYPEC_TBT_SID		USB_TYPEC_VENDOR_INTEL


#define TYPEC_TBT_MODE			TYPEC_STATE_MODAL


struct typec_thunderbolt_data {
	u32 device_mode;
	u32 cable_mode;
	u32 enter_vdo;
};


#define TBT_MODE			BIT(0)
#define TBT_ADAPTER(_vdo_)		(((_vdo_) & BIT(16)) >> 16)
#define   TBT_ADAPTER_LEGACY		0
#define   TBT_ADAPTER_TBT3		1
#define TBT_INTEL_SPECIFIC_B0		BIT(26)
#define TBT_VENDOR_SPECIFIC_B0		BIT(30)
#define TBT_VENDOR_SPECIFIC_B1		BIT(31)

#define TBT_SET_ADAPTER(a)		(((a) & 1) << 16)


#define TBT_CABLE_SPEED(_vdo_)		(((_vdo_) & GENMASK(18, 16)) >> 16)
#define   TBT_CABLE_USB3_GEN1		1
#define   TBT_CABLE_USB3_PASSIVE	2
#define   TBT_CABLE_10_AND_20GBPS	3
#define TBT_CABLE_ROUNDED		BIT(19)
#define TBT_CABLE_OPTICAL		BIT(21)
#define TBT_CABLE_RETIMER		BIT(22)
#define TBT_CABLE_LINK_TRAINING		BIT(23)

#define TBT_SET_CABLE_SPEED(_s_)	(((_s_) & GENMASK(2, 0)) << 16)


#define TBT_ENTER_MODE_CABLE_SPEED(s)	TBT_SET_CABLE_SPEED(s)
#define TBT_ENTER_MODE_ACTIVE_CABLE	BIT(24)

#endif 
