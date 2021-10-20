/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _DW_EDMA_H
#define _DW_EDMA_H

#include <linux/device.h>
#include <linux/dmaengine.h>

struct dw_edma;


struct dw_edma_chip {
	struct device		*dev;
	int			id;
	int			irq;
	struct dw_edma		*dw;
};


#if IS_ENABLED(CONFIG_DW_EDMA)
int dw_edma_probe(struct dw_edma_chip *chip);
int dw_edma_remove(struct dw_edma_chip *chip);
#else
static inline int dw_edma_probe(struct dw_edma_chip *chip)
{
	return -ENODEV;
}

static inline int dw_edma_remove(struct dw_edma_chip *chip)
{
	return 0;
}
#endif 

#endif 
