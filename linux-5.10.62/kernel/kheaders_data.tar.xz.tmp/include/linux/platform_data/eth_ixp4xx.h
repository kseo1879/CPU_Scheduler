/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __PLATFORM_DATA_ETH_IXP4XX
#define __PLATFORM_DATA_ETH_IXP4XX

#include <linux/types.h>

#define IXP4XX_ETH_NPEA		0x00
#define IXP4XX_ETH_NPEB		0x10
#define IXP4XX_ETH_NPEC		0x20


struct eth_plat_info {
	u8 phy;		
	u8 rxq;		
	u8 txreadyq;
	u8 hwaddr[6];
};

#endif
