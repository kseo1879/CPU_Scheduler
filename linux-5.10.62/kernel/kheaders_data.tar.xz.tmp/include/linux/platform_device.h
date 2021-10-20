/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _PLATFORM_DEVICE_H_
#define _PLATFORM_DEVICE_H_

#include <linux/device.h>

#define PLATFORM_DEVID_NONE	(-1)
#define PLATFORM_DEVID_AUTO	(-2)

struct mfd_cell;
struct property_entry;
struct platform_device_id;

struct platform_device {
	const char	*name;
	int		id;
	bool		id_auto;
	struct device	dev;
	u64		platform_dma_mask;
	struct device_dma_parameters dma_parms;
	u32		num_resources;
	struct resource	*resource;

	const struct platform_device_id	*id_entry;
	char *driver_override; 

	
	struct mfd_cell *mfd_cell;

	
	struct pdev_archdata	archdata;
};

#define platform_get_device_id(pdev)	((pdev)->id_entry)

#define dev_is_platform(dev) ((dev)->bus == &platform_bus_type)
#define to_platform_device(x) container_of((x), struct platform_device, dev)

extern int platform_device_register(struct platform_device *);
extern void platform_device_unregister(struct platform_device *);

extern struct bus_type platform_bus_type;
extern struct device platform_bus;

extern struct resource *platform_get_resource(struct platform_device *,
					      unsigned int, unsigned int);
extern struct device *
platform_find_device_by_driver(struct device *start,
			       const struct device_driver *drv);
extern void __iomem *
devm_platform_get_and_ioremap_resource(struct platform_device *pdev,
				unsigned int index, struct resource **res);
extern void __iomem *
devm_platform_ioremap_resource(struct platform_device *pdev,
			       unsigned int index);
extern void __iomem *
devm_platform_ioremap_resource_wc(struct platform_device *pdev,
				  unsigned int index);
extern void __iomem *
devm_platform_ioremap_resource_byname(struct platform_device *pdev,
				      const char *name);
extern int platform_get_irq(struct platform_device *, unsigned int);
extern int platform_get_irq_optional(struct platform_device *, unsigned int);
extern int platform_irq_count(struct platform_device *);
extern struct resource *platform_get_resource_byname(struct platform_device *,
						     unsigned int,
						     const char *);
extern int platform_get_irq_byname(struct platform_device *, const char *);
extern int platform_get_irq_byname_optional(struct platform_device *dev,
					    const char *name);
extern int platform_add_devices(struct platform_device **, int);

struct platform_device_info {
		struct device *parent;
		struct fwnode_handle *fwnode;
		bool of_node_reused;

		const char *name;
		int id;

		const struct resource *res;
		unsigned int num_res;

		const void *data;
		size_t size_data;
		u64 dma_mask;

		const struct property_entry *properties;
};
extern struct platform_device *platform_device_register_full(
		const struct platform_device_info *pdevinfo);


static inline struct platform_device *platform_device_register_resndata(
		struct device *parent, const char *name, int id,
		const struct resource *res, unsigned int num,
		const void *data, size_t size) {

	struct platform_device_info pdevinfo = {
		.parent = parent,
		.name = name,
		.id = id,
		.res = res,
		.num_res = num,
		.data = data,
		.size_data = size,
		.dma_mask = 0,
	};

	return platform_device_register_full(&pdevinfo);
}


static inline struct platform_device *platform_device_register_simple(
		const char *name, int id,
		const struct resource *res, unsigned int num)
{
	return platform_device_register_resndata(NULL, name, id,
			res, num, NULL, 0);
}


static inline struct platform_device *platform_device_register_data(
		struct device *parent, const char *name, int id,
		const void *data, size_t size)
{
	return platform_device_register_resndata(parent, name, id,
			NULL, 0, data, size);
}

extern struct platform_device *platform_device_alloc(const char *name, int id);
extern int platform_device_add_resources(struct platform_device *pdev,
					 const struct resource *res,
					 unsigned int num);
extern int platform_device_add_data(struct platform_device *pdev,
				    const void *data, size_t size);
extern int platform_device_add_properties(struct platform_device *pdev,
				const struct property_entry *properties);
extern int platform_device_add(struct platform_device *pdev);
extern void platform_device_del(struct platform_device *pdev);
extern void platform_device_put(struct platform_device *pdev);

struct platform_driver {
	int (*probe)(struct platform_device *);
	int (*remove)(struct platform_device *);
	void (*shutdown)(struct platform_device *);
	int (*suspend)(struct platform_device *, pm_message_t state);
	int (*resume)(struct platform_device *);
	struct device_driver driver;
	const struct platform_device_id *id_table;
	bool prevent_deferred_probe;
};

#define to_platform_driver(drv)	(container_of((drv), struct platform_driver, \
				 driver))


#define platform_driver_register(drv) \
	__platform_driver_register(drv, THIS_MODULE)
extern int __platform_driver_register(struct platform_driver *,
					struct module *);
extern void platform_driver_unregister(struct platform_driver *);


#define platform_driver_probe(drv, probe) \
	__platform_driver_probe(drv, probe, THIS_MODULE)
extern int __platform_driver_probe(struct platform_driver *driver,
		int (*probe)(struct platform_device *), struct module *module);

static inline void *platform_get_drvdata(const struct platform_device *pdev)
{
	return dev_get_drvdata(&pdev->dev);
}

static inline void platform_set_drvdata(struct platform_device *pdev,
					void *data)
{
	dev_set_drvdata(&pdev->dev, data);
}


#define module_platform_driver(__platform_driver) \
	module_driver(__platform_driver, platform_driver_register, \
			platform_driver_unregister)


#define builtin_platform_driver(__platform_driver) \
	builtin_driver(__platform_driver, platform_driver_register)


#define module_platform_driver_probe(__platform_driver, __platform_probe) \
static int __init __platform_driver##_init(void) \
{ \
	return platform_driver_probe(&(__platform_driver), \
				     __platform_probe);    \
} \
module_init(__platform_driver##_init); \
static void __exit __platform_driver##_exit(void) \
{ \
	platform_driver_unregister(&(__platform_driver)); \
} \
module_exit(__platform_driver##_exit);


#define builtin_platform_driver_probe(__platform_driver, __platform_probe) \
static int __init __platform_driver##_init(void) \
{ \
	return platform_driver_probe(&(__platform_driver), \
				     __platform_probe);    \
} \
device_initcall(__platform_driver##_init); \

#define platform_create_bundle(driver, probe, res, n_res, data, size) \
	__platform_create_bundle(driver, probe, res, n_res, data, size, THIS_MODULE)
extern struct platform_device *__platform_create_bundle(
	struct platform_driver *driver, int (*probe)(struct platform_device *),
	struct resource *res, unsigned int n_res,
	const void *data, size_t size, struct module *module);

int __platform_register_drivers(struct platform_driver * const *drivers,
				unsigned int count, struct module *owner);
void platform_unregister_drivers(struct platform_driver * const *drivers,
				 unsigned int count);

#define platform_register_drivers(drivers, count) \
	__platform_register_drivers(drivers, count, THIS_MODULE)

#ifdef CONFIG_SUSPEND
extern int platform_pm_suspend(struct device *dev);
extern int platform_pm_resume(struct device *dev);
#else
#define platform_pm_suspend		NULL
#define platform_pm_resume		NULL
#endif

#ifdef CONFIG_HIBERNATE_CALLBACKS
extern int platform_pm_freeze(struct device *dev);
extern int platform_pm_thaw(struct device *dev);
extern int platform_pm_poweroff(struct device *dev);
extern int platform_pm_restore(struct device *dev);
#else
#define platform_pm_freeze		NULL
#define platform_pm_thaw		NULL
#define platform_pm_poweroff		NULL
#define platform_pm_restore		NULL
#endif

extern int platform_dma_configure(struct device *dev);

#ifdef CONFIG_PM_SLEEP
#define USE_PLATFORM_PM_SLEEP_OPS \
	.suspend = platform_pm_suspend, \
	.resume = platform_pm_resume, \
	.freeze = platform_pm_freeze, \
	.thaw = platform_pm_thaw, \
	.poweroff = platform_pm_poweroff, \
	.restore = platform_pm_restore,
#else
#define USE_PLATFORM_PM_SLEEP_OPS
#endif

#ifndef CONFIG_SUPERH

static inline int is_sh_early_platform_device(struct platform_device *pdev)
{
	return 0;
}
#endif 


void early_platform_cleanup(void);

#endif 
