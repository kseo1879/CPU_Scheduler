/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef V4L2_ASYNC_H
#define V4L2_ASYNC_H

#include <linux/list.h>
#include <linux/mutex.h>

struct device;
struct device_node;
struct v4l2_device;
struct v4l2_subdev;
struct v4l2_async_notifier;


enum v4l2_async_match_type {
	V4L2_ASYNC_MATCH_CUSTOM,
	V4L2_ASYNC_MATCH_DEVNAME,
	V4L2_ASYNC_MATCH_I2C,
	V4L2_ASYNC_MATCH_FWNODE,
};


struct v4l2_async_subdev {
	enum v4l2_async_match_type match_type;
	union {
		struct fwnode_handle *fwnode;
		const char *device_name;
		struct {
			int adapter_id;
			unsigned short address;
		} i2c;
		struct {
			bool (*match)(struct device *dev,
				      struct v4l2_async_subdev *sd);
			void *priv;
		} custom;
	} match;

	
	struct list_head list;
	struct list_head asd_list;
};


struct v4l2_async_notifier_operations {
	int (*bound)(struct v4l2_async_notifier *notifier,
		     struct v4l2_subdev *subdev,
		     struct v4l2_async_subdev *asd);
	int (*complete)(struct v4l2_async_notifier *notifier);
	void (*unbind)(struct v4l2_async_notifier *notifier,
		       struct v4l2_subdev *subdev,
		       struct v4l2_async_subdev *asd);
};


struct v4l2_async_notifier {
	const struct v4l2_async_notifier_operations *ops;
	struct v4l2_device *v4l2_dev;
	struct v4l2_subdev *sd;
	struct v4l2_async_notifier *parent;
	struct list_head asd_list;
	struct list_head waiting;
	struct list_head done;
	struct list_head list;
};


void v4l2_async_notifier_init(struct v4l2_async_notifier *notifier);


int v4l2_async_notifier_add_subdev(struct v4l2_async_notifier *notifier,
				   struct v4l2_async_subdev *asd);


struct v4l2_async_subdev *
v4l2_async_notifier_add_fwnode_subdev(struct v4l2_async_notifier *notifier,
				      struct fwnode_handle *fwnode,
				      unsigned int asd_struct_size);


struct v4l2_async_subdev *
v4l2_async_notifier_add_fwnode_remote_subdev(struct v4l2_async_notifier *notif,
					     struct fwnode_handle *endpoint,
					     unsigned int asd_struct_size);


struct v4l2_async_subdev *
v4l2_async_notifier_add_i2c_subdev(struct v4l2_async_notifier *notifier,
				   int adapter_id, unsigned short address,
				   unsigned int asd_struct_size);


struct v4l2_async_subdev *
v4l2_async_notifier_add_devname_subdev(struct v4l2_async_notifier *notifier,
				       const char *device_name,
				       unsigned int asd_struct_size);


int v4l2_async_notifier_register(struct v4l2_device *v4l2_dev,
				 struct v4l2_async_notifier *notifier);


int v4l2_async_subdev_notifier_register(struct v4l2_subdev *sd,
					struct v4l2_async_notifier *notifier);


void v4l2_async_notifier_unregister(struct v4l2_async_notifier *notifier);


void v4l2_async_notifier_cleanup(struct v4l2_async_notifier *notifier);


int v4l2_async_register_subdev(struct v4l2_subdev *sd);


int __must_check
v4l2_async_register_subdev_sensor_common(struct v4l2_subdev *sd);


void v4l2_async_unregister_subdev(struct v4l2_subdev *sd);
#endif
