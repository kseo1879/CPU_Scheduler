/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */


#ifndef __SDW_INTEL_H
#define __SDW_INTEL_H

#include <linux/irqreturn.h>
#include <linux/soundwire/sdw.h>


struct sdw_intel_stream_params_data {
	struct snd_pcm_substream *substream;
	struct snd_soc_dai *dai;
	struct snd_pcm_hw_params *hw_params;
	int link_id;
	int alh_stream_id;
};


struct sdw_intel_stream_free_data {
	struct snd_pcm_substream *substream;
	struct snd_soc_dai *dai;
	int link_id;
};


struct sdw_intel_ops {
	int (*params_stream)(struct device *dev,
			     struct sdw_intel_stream_params_data *params_data);
	int (*free_stream)(struct device *dev,
			   struct sdw_intel_stream_free_data *free_data);
};


struct sdw_intel_acpi_info {
	acpi_handle handle;
	int count;
	u32 link_mask;
};

struct sdw_intel_link_res;




#define SDW_INTEL_CLK_STOP_NOT_ALLOWED		BIT(0)


#define SDW_INTEL_CLK_STOP_TEARDOWN		BIT(1)


#define SDW_INTEL_CLK_STOP_WAKE_CAPABLE_ONLY	BIT(2)


#define SDW_INTEL_CLK_STOP_BUS_RESET		BIT(3)

struct sdw_intel_slave_id {
	int link_id;
	struct sdw_slave_id id;
};


struct sdw_intel_ctx {
	int count;
	void __iomem *mmio_base;
	u32 link_mask;
	int num_slaves;
	acpi_handle handle;
	struct sdw_intel_link_res *links;
	struct sdw_intel_slave_id *ids;
	struct list_head link_list;
	struct mutex shim_lock; 
	u32 shim_mask;
};


struct sdw_intel_res {
	int count;
	void __iomem *mmio_base;
	int irq;
	acpi_handle handle;
	struct device *parent;
	const struct sdw_intel_ops *ops;
	struct device *dev;
	u32 link_mask;
	u32 clock_stop_quirks;
};


int sdw_intel_acpi_scan(acpi_handle *parent_handle,
			struct sdw_intel_acpi_info *info);

void sdw_intel_process_wakeen_event(struct sdw_intel_ctx *ctx);

struct sdw_intel_ctx *
sdw_intel_probe(struct sdw_intel_res *res);

int sdw_intel_startup(struct sdw_intel_ctx *ctx);

void sdw_intel_exit(struct sdw_intel_ctx *ctx);

void sdw_intel_enable_irq(void __iomem *mmio_base, bool enable);

irqreturn_t sdw_intel_thread(int irq, void *dev_id);

#endif
