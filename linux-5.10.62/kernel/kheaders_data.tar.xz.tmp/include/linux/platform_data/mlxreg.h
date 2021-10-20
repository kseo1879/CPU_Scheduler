/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0 */


#ifndef __LINUX_PLATFORM_DATA_MLXREG_H
#define __LINUX_PLATFORM_DATA_MLXREG_H

#define MLXREG_CORE_LABEL_MAX_SIZE	32
#define MLXREG_CORE_WD_FEATURE_NOWAYOUT		BIT(0)
#define MLXREG_CORE_WD_FEATURE_START_AT_BOOT	BIT(1)


enum mlxreg_wdt_type {
	MLX_WDT_TYPE1,
	MLX_WDT_TYPE2,
	MLX_WDT_TYPE3,
};


struct mlxreg_hotplug_device {
	struct i2c_adapter *adapter;
	struct i2c_client *client;
	struct i2c_board_info *brdinfo;
	int nr;
};


struct mlxreg_core_data {
	char label[MLXREG_CORE_LABEL_MAX_SIZE];
	u32 reg;
	u32 mask;
	u32 bit;
	u32 capability;
	u32 reg_prsnt;
	umode_t	mode;
	struct device_node *np;
	struct mlxreg_hotplug_device hpdev;
	u32 health_cntr;
	bool attached;
	u8 regnum;
};


struct mlxreg_core_item {
	struct mlxreg_core_data *data;
	u32 aggr_mask;
	u32 reg;
	u32 mask;
	u32 capability;
	u32 cache;
	u8 count;
	u8 ind;
	u8 inversed;
	u8 health;
};


struct mlxreg_core_platform_data {
	struct mlxreg_core_data *data;
	void *regmap;
	int counter;
	u32 features;
	u32 version;
	char identity[MLXREG_CORE_LABEL_MAX_SIZE];
	u32 capability;
};


struct mlxreg_core_hotplug_platform_data {
	struct mlxreg_core_item *items;
	int irq;
	void *regmap;
	int counter;
	u32 cell;
	u32 mask;
	u32 cell_low;
	u32 mask_low;
	int deferred_nr;
	int shift_nr;
};

#endif 
