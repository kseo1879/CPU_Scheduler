/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _LINUX_NVMEM_PROVIDER_H
#define _LINUX_NVMEM_PROVIDER_H

#include <linux/err.h>
#include <linux/errno.h>
#include <linux/gpio/consumer.h>

struct nvmem_device;
struct nvmem_cell_info;
typedef int (*nvmem_reg_read_t)(void *priv, unsigned int offset,
				void *val, size_t bytes);
typedef int (*nvmem_reg_write_t)(void *priv, unsigned int offset,
				 void *val, size_t bytes);

enum nvmem_type {
	NVMEM_TYPE_UNKNOWN = 0,
	NVMEM_TYPE_EEPROM,
	NVMEM_TYPE_OTP,
	NVMEM_TYPE_BATTERY_BACKED,
};

#define NVMEM_DEVID_NONE	(-1)
#define NVMEM_DEVID_AUTO	(-2)


struct nvmem_config {
	struct device		*dev;
	const char		*name;
	int			id;
	struct module		*owner;
	struct gpio_desc	*wp_gpio;
	const struct nvmem_cell_info	*cells;
	int			ncells;
	enum nvmem_type		type;
	bool			read_only;
	bool			root_only;
	bool			no_of_node;
	nvmem_reg_read_t	reg_read;
	nvmem_reg_write_t	reg_write;
	int	size;
	int	word_size;
	int	stride;
	void	*priv;
	
	bool			compat;
	struct device		*base_dev;
};


struct nvmem_cell_table {
	const char		*nvmem_name;
	const struct nvmem_cell_info	*cells;
	size_t			ncells;
	struct list_head	node;
};

#if IS_ENABLED(CONFIG_NVMEM)

struct nvmem_device *nvmem_register(const struct nvmem_config *cfg);
void nvmem_unregister(struct nvmem_device *nvmem);

struct nvmem_device *devm_nvmem_register(struct device *dev,
					 const struct nvmem_config *cfg);

int devm_nvmem_unregister(struct device *dev, struct nvmem_device *nvmem);

void nvmem_add_cell_table(struct nvmem_cell_table *table);
void nvmem_del_cell_table(struct nvmem_cell_table *table);

#else

static inline struct nvmem_device *nvmem_register(const struct nvmem_config *c)
{
	return ERR_PTR(-EOPNOTSUPP);
}

static inline void nvmem_unregister(struct nvmem_device *nvmem) {}

static inline struct nvmem_device *
devm_nvmem_register(struct device *dev, const struct nvmem_config *c)
{
	return nvmem_register(c);
}

static inline int
devm_nvmem_unregister(struct device *dev, struct nvmem_device *nvmem)
{
	return -EOPNOTSUPP;
}

static inline void nvmem_add_cell_table(struct nvmem_cell_table *table) {}
static inline void nvmem_del_cell_table(struct nvmem_cell_table *table) {}

#endif 
#endif  
