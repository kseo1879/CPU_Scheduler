/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_VDPA_H
#define _LINUX_VDPA_H

#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/vhost_iotlb.h>


struct vdpa_callback {
	irqreturn_t (*callback)(void *data);
	void *private;
};


struct vdpa_notification_area {
	resource_size_t addr;
	resource_size_t size;
};


struct vdpa_vq_state {
	u16	avail_index;
};


struct vdpa_device {
	struct device dev;
	struct device *dma_dev;
	const struct vdpa_config_ops *config;
	unsigned int index;
	bool features_valid;
	int nvqs;
};


struct vdpa_iova_range {
	u64 first;
	u64 last;
};


struct vdpa_config_ops {
	
	int (*set_vq_address)(struct vdpa_device *vdev,
			      u16 idx, u64 desc_area, u64 driver_area,
			      u64 device_area);
	void (*set_vq_num)(struct vdpa_device *vdev, u16 idx, u32 num);
	void (*kick_vq)(struct vdpa_device *vdev, u16 idx);
	void (*set_vq_cb)(struct vdpa_device *vdev, u16 idx,
			  struct vdpa_callback *cb);
	void (*set_vq_ready)(struct vdpa_device *vdev, u16 idx, bool ready);
	bool (*get_vq_ready)(struct vdpa_device *vdev, u16 idx);
	int (*set_vq_state)(struct vdpa_device *vdev, u16 idx,
			    const struct vdpa_vq_state *state);
	int (*get_vq_state)(struct vdpa_device *vdev, u16 idx,
			    struct vdpa_vq_state *state);
	struct vdpa_notification_area
	(*get_vq_notification)(struct vdpa_device *vdev, u16 idx);
	
	int (*get_vq_irq)(struct vdpa_device *vdv, u16 idx);

	
	u32 (*get_vq_align)(struct vdpa_device *vdev);
	u64 (*get_features)(struct vdpa_device *vdev);
	int (*set_features)(struct vdpa_device *vdev, u64 features);
	void (*set_config_cb)(struct vdpa_device *vdev,
			      struct vdpa_callback *cb);
	u16 (*get_vq_num_max)(struct vdpa_device *vdev);
	u32 (*get_device_id)(struct vdpa_device *vdev);
	u32 (*get_vendor_id)(struct vdpa_device *vdev);
	u8 (*get_status)(struct vdpa_device *vdev);
	void (*set_status)(struct vdpa_device *vdev, u8 status);
	void (*get_config)(struct vdpa_device *vdev, unsigned int offset,
			   void *buf, unsigned int len);
	void (*set_config)(struct vdpa_device *vdev, unsigned int offset,
			   const void *buf, unsigned int len);
	u32 (*get_generation)(struct vdpa_device *vdev);
	struct vdpa_iova_range (*get_iova_range)(struct vdpa_device *vdev);

	
	int (*set_map)(struct vdpa_device *vdev, struct vhost_iotlb *iotlb);
	int (*dma_map)(struct vdpa_device *vdev, u64 iova, u64 size,
		       u64 pa, u32 perm);
	int (*dma_unmap)(struct vdpa_device *vdev, u64 iova, u64 size);

	
	void (*free)(struct vdpa_device *vdev);
};

struct vdpa_device *__vdpa_alloc_device(struct device *parent,
					const struct vdpa_config_ops *config,
					int nvqs,
					size_t size);

#define vdpa_alloc_device(dev_struct, member, parent, config, nvqs)   \
			  container_of(__vdpa_alloc_device( \
				       parent, config, nvqs, \
				       sizeof(dev_struct) + \
				       BUILD_BUG_ON_ZERO(offsetof( \
				       dev_struct, member))), \
				       dev_struct, member)

int vdpa_register_device(struct vdpa_device *vdev);
void vdpa_unregister_device(struct vdpa_device *vdev);


struct vdpa_driver {
	struct device_driver driver;
	int (*probe)(struct vdpa_device *vdev);
	void (*remove)(struct vdpa_device *vdev);
};

#define vdpa_register_driver(drv) \
	__vdpa_register_driver(drv, THIS_MODULE)
int __vdpa_register_driver(struct vdpa_driver *drv, struct module *owner);
void vdpa_unregister_driver(struct vdpa_driver *drv);

#define module_vdpa_driver(__vdpa_driver) \
	module_driver(__vdpa_driver, vdpa_register_driver,	\
		      vdpa_unregister_driver)

static inline struct vdpa_driver *drv_to_vdpa(struct device_driver *driver)
{
	return container_of(driver, struct vdpa_driver, driver);
}

static inline struct vdpa_device *dev_to_vdpa(struct device *_dev)
{
	return container_of(_dev, struct vdpa_device, dev);
}

static inline void *vdpa_get_drvdata(const struct vdpa_device *vdev)
{
	return dev_get_drvdata(&vdev->dev);
}

static inline void vdpa_set_drvdata(struct vdpa_device *vdev, void *data)
{
	dev_set_drvdata(&vdev->dev, data);
}

static inline struct device *vdpa_get_dma_dev(struct vdpa_device *vdev)
{
	return vdev->dma_dev;
}

static inline void vdpa_reset(struct vdpa_device *vdev)
{
        const struct vdpa_config_ops *ops = vdev->config;

	vdev->features_valid = false;
        ops->set_status(vdev, 0);
}

static inline int vdpa_set_features(struct vdpa_device *vdev, u64 features)
{
        const struct vdpa_config_ops *ops = vdev->config;

	vdev->features_valid = true;
        return ops->set_features(vdev, features);
}


static inline void vdpa_get_config(struct vdpa_device *vdev, unsigned offset,
				   void *buf, unsigned int len)
{
        const struct vdpa_config_ops *ops = vdev->config;

	
	if (!vdev->features_valid)
		vdpa_set_features(vdev, 0);
	ops->get_config(vdev, offset, buf, len);
}

#endif 
