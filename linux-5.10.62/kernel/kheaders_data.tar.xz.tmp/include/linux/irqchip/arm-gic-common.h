/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __LINUX_IRQCHIP_ARM_GIC_COMMON_H
#define __LINUX_IRQCHIP_ARM_GIC_COMMON_H

#include <linux/types.h>
#include <linux/ioport.h>

#define GICD_INT_DEF_PRI		0xa0
#define GICD_INT_DEF_PRI_X4		((GICD_INT_DEF_PRI << 24) |\
					(GICD_INT_DEF_PRI << 16) |\
					(GICD_INT_DEF_PRI << 8) |\
					GICD_INT_DEF_PRI)

enum gic_type {
	GIC_V2,
	GIC_V3,
};

struct gic_kvm_info {
	
	enum gic_type	type;
	
	struct resource vcpu;
	
	unsigned int	maint_irq;
	
	struct resource vctrl;
	
	bool		has_v4;
	
	bool		has_v4_1;
};

const struct gic_kvm_info *gic_get_kvm_info(void);

struct irq_domain;
struct fwnode_handle;
int gicv2m_init(struct fwnode_handle *parent_handle,
		struct irq_domain *parent);

#endif 
