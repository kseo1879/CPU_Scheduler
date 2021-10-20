/* SPDX-License-Identifier: GPL-2.0 */

#ifndef AMBA_MMCI_H
#define AMBA_MMCI_H

#include <linux/mmc/host.h>


struct mmci_platform_data {
	unsigned int ocr_mask;
	int (*ios_handler)(struct device *, struct mmc_ios *);
	unsigned int (*status)(struct device *);
};

#endif
