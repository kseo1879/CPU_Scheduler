/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _LINUX_PLATFORM_DATA_TI_PRM_H
#define _LINUX_PLATFORM_DATA_TI_PRM_H

struct clockdomain;

struct ti_prm_platform_data {
	void (*clkdm_deny_idle)(struct clockdomain *clkdm);
	void (*clkdm_allow_idle)(struct clockdomain *clkdm);
	struct clockdomain * (*clkdm_lookup)(const char *name);
};

#endif 
