/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef __LINUX_USB_PD_VDO_H
#define __LINUX_USB_PD_VDO_H

#include "pd.h"



#define VDO_MAX_OBJECTS		6
#define VDO_MAX_SIZE		(VDO_MAX_OBJECTS + 1)


#define VDO(vid, type, custom)				\
	(((vid) << 16) |				\
	 ((type) << 15) |				\
	 ((custom) & 0x7FFF))

#define VDO_SVDM_TYPE		(1 << 15)
#define VDO_SVDM_VERS(x)	((x) << 13)
#define VDO_OPOS(x)		((x) << 8)
#define VDO_CMDT(x)		((x) << 6)
#define VDO_OPOS_MASK		VDO_OPOS(0x7)
#define VDO_CMDT_MASK		VDO_CMDT(0x3)

#define CMDT_INIT		0
#define CMDT_RSP_ACK		1
#define CMDT_RSP_NAK		2
#define CMDT_RSP_BUSY		3


#define VDO_SRC_INITIATOR	(0 << 5)
#define VDO_SRC_RESPONDER	(1 << 5)

#define CMD_DISCOVER_IDENT	1
#define CMD_DISCOVER_SVID	2
#define CMD_DISCOVER_MODES	3
#define CMD_ENTER_MODE		4
#define CMD_EXIT_MODE		5
#define CMD_ATTENTION		6

#define VDO_CMD_VENDOR(x)    (((0x10 + (x)) & 0x1f))


#define VDO_CMD_VERSION		VDO_CMD_VENDOR(0)
#define VDO_CMD_SEND_INFO	VDO_CMD_VENDOR(1)
#define VDO_CMD_READ_INFO	VDO_CMD_VENDOR(2)
#define VDO_CMD_REBOOT		VDO_CMD_VENDOR(5)
#define VDO_CMD_FLASH_ERASE	VDO_CMD_VENDOR(6)
#define VDO_CMD_FLASH_WRITE	VDO_CMD_VENDOR(7)
#define VDO_CMD_ERASE_SIG	VDO_CMD_VENDOR(8)
#define VDO_CMD_PING_ENABLE	VDO_CMD_VENDOR(10)
#define VDO_CMD_CURRENT		VDO_CMD_VENDOR(11)
#define VDO_CMD_FLIP		VDO_CMD_VENDOR(12)
#define VDO_CMD_GET_LOG		VDO_CMD_VENDOR(13)
#define VDO_CMD_CCD_EN		VDO_CMD_VENDOR(14)

#define PD_VDO_VID(vdo)		((vdo) >> 16)
#define PD_VDO_SVDM(vdo)	(((vdo) >> 15) & 1)
#define PD_VDO_OPOS(vdo)	(((vdo) >> 8) & 0x7)
#define PD_VDO_CMD(vdo)		((vdo) & 0x1f)
#define PD_VDO_CMDT(vdo)	(((vdo) >> 6) & 0x3)


#define VDO_INDEX_HDR		0
#define VDO_INDEX_IDH		1
#define VDO_INDEX_CSTAT		2
#define VDO_INDEX_CABLE		3
#define VDO_INDEX_PRODUCT	3
#define VDO_INDEX_AMA		4


#define IDH_PTYPE_UNDEF		0
#define IDH_PTYPE_HUB		1
#define IDH_PTYPE_PERIPH	2
#define IDH_PTYPE_PCABLE	3
#define IDH_PTYPE_ACABLE	4
#define IDH_PTYPE_AMA		5

#define VDO_IDH(usbh, usbd, ptype, is_modal, vid)		\
	((usbh) << 31 | (usbd) << 30 | ((ptype) & 0x7) << 27	\
	 | (is_modal) << 26 | ((vid) & 0xffff))

#define PD_IDH_PTYPE(vdo)	(((vdo) >> 27) & 0x7)
#define PD_IDH_VID(vdo)		((vdo) & 0xffff)
#define PD_IDH_MODAL_SUPP(vdo)	((vdo) & (1 << 26))


#define PD_CSTAT_XID(vdo)	(vdo)


#define VDO_PRODUCT(pid, bcd)	(((pid) & 0xffff) << 16 | ((bcd) & 0xffff))
#define PD_PRODUCT_PID(vdo)	(((vdo) >> 16) & 0xffff)


#define PD_VDO1_UFP_DEVCAP(vdo)	(((vdo) & GENMASK(27, 24)) >> 24)

#define DEV_USB2_CAPABLE	BIT(0)
#define DEV_USB2_BILLBOARD	BIT(1)
#define DEV_USB3_CAPABLE	BIT(2)
#define DEV_USB4_CAPABLE	BIT(3)


#define PD_VDO_DFP_HOSTCAP(vdo)	(((vdo) & GENMASK(26, 24)) >> 24)

#define HOST_USB2_CAPABLE	BIT(0)
#define HOST_USB3_CAPABLE	BIT(1)
#define HOST_USB4_CAPABLE	BIT(2)


#define CABLE_ATYPE		0
#define CABLE_BTYPE		1
#define CABLE_CTYPE		2
#define CABLE_PLUG		0
#define CABLE_RECEPTACLE	1
#define CABLE_CURR_1A5		0
#define CABLE_CURR_3A		1
#define CABLE_CURR_5A		2
#define CABLE_USBSS_U2_ONLY	0
#define CABLE_USBSS_U31_GEN1	1
#define CABLE_USBSS_U31_GEN2	2
#define VDO_CABLE(hw, fw, cbl, gdr, lat, term, tx1d, tx2d, rx1d, rx2d, cur,\
		  vps, sopp, usbss) \
	(((hw) & 0x7) << 28 | ((fw) & 0x7) << 24 | ((cbl) & 0x3) << 18	\
	 | (gdr) << 17 | ((lat) & 0x7) << 13 | ((term) & 0x3) << 11	\
	 | (tx1d) << 10 | (tx2d) << 9 | (rx1d) << 8 | (rx2d) << 7	\
	 | ((cur) & 0x3) << 5 | (vps) << 4 | (sopp) << 3		\
	 | ((usbss) & 0x7))


#define VDO_AMA(hw, fw, tx1d, tx2d, rx1d, rx2d, vcpwr, vcr, vbr, usbss) \
	(((hw) & 0x7) << 28 | ((fw) & 0x7) << 24			\
	 | (tx1d) << 11 | (tx2d) << 10 | (rx1d) << 9 | (rx2d) << 8	\
	 | ((vcpwr) & 0x7) << 5 | (vcr) << 4 | (vbr) << 3		\
	 | ((usbss) & 0x7))

#define PD_VDO_AMA_VCONN_REQ(vdo)	(((vdo) >> 4) & 1)
#define PD_VDO_AMA_VBUS_REQ(vdo)	(((vdo) >> 3) & 1)

#define AMA_VCONN_PWR_1W	0
#define AMA_VCONN_PWR_1W5	1
#define AMA_VCONN_PWR_2W	2
#define AMA_VCONN_PWR_3W	3
#define AMA_VCONN_PWR_4W	4
#define AMA_VCONN_PWR_5W	5
#define AMA_VCONN_PWR_6W	6
#define AMA_USBSS_U2_ONLY	0
#define AMA_USBSS_U31_GEN1	1
#define AMA_USBSS_U31_GEN2	2
#define AMA_USBSS_BBONLY	3


#define VDO_SVID(svid0, svid1)	(((svid0) & 0xffff) << 16 | ((svid1) & 0xffff))
#define PD_VDO_SVID_SVID0(vdo)	((vdo) >> 16)
#define PD_VDO_SVID_SVID1(vdo)	((vdo) & 0xffff)


#define USB_SID_PD		0xff00 
#define USB_SID_DISPLAYPORT	0xff01
#define USB_SID_MHL		0xff02	



#define PD_T_VDM_UNSTRUCTURED	500
#define PD_T_VDM_BUSY		100
#define PD_T_VDM_WAIT_MODE_E	100
#define PD_T_VDM_SNDR_RSP	30
#define PD_T_VDM_E_MODE		25
#define PD_T_VDM_RCVR_RSP	15

#endif 
