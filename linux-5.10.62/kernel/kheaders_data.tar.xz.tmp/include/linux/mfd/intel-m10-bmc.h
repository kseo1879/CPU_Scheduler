/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __MFD_INTEL_M10_BMC_H
#define __MFD_INTEL_M10_BMC_H

#include <linux/regmap.h>

#define M10BMC_LEGACY_SYS_BASE		0x300400
#define M10BMC_SYS_BASE			0x300800
#define M10BMC_MEM_END			0x1fffffff


#define NIOS2_FW_VERSION		0x0
#define M10BMC_TEST_REG			0x3c
#define M10BMC_BUILD_VER		0x68
#define M10BMC_VER_MAJOR_MSK		GENMASK(23, 16)
#define M10BMC_VER_PCB_INFO_MSK		GENMASK(31, 24)
#define M10BMC_VER_LEGACY_INVALID	0xffffffff


struct intel_m10bmc {
	struct device *dev;
	struct regmap *regmap;
};


static inline int
m10bmc_raw_read(struct intel_m10bmc *m10bmc, unsigned int addr,
		unsigned int *val)
{
	int ret;

	ret = regmap_read(m10bmc->regmap, addr, val);
	if (ret)
		dev_err(m10bmc->dev, "fail to read raw reg %x: %d\n",
			addr, ret);

	return ret;
}


#define m10bmc_sys_read(m10bmc, offset, val) \
	m10bmc_raw_read(m10bmc, M10BMC_SYS_BASE + (offset), val)

#endif 
