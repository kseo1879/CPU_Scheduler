/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _V4L2_FWNODE_H
#define _V4L2_FWNODE_H

#include <linux/errno.h>
#include <linux/fwnode.h>
#include <linux/list.h>
#include <linux/types.h>

#include <media/v4l2-mediabus.h>

struct fwnode_handle;
struct v4l2_async_notifier;
struct v4l2_async_subdev;

#define V4L2_FWNODE_CSI2_MAX_DATA_LANES	4


struct v4l2_fwnode_bus_mipi_csi2 {
	unsigned int flags;
	unsigned char data_lanes[V4L2_FWNODE_CSI2_MAX_DATA_LANES];
	unsigned char clock_lane;
	unsigned char num_data_lanes;
	bool lane_polarities[1 + V4L2_FWNODE_CSI2_MAX_DATA_LANES];
};


struct v4l2_fwnode_bus_parallel {
	unsigned int flags;
	unsigned char bus_width;
	unsigned char data_shift;
};


struct v4l2_fwnode_bus_mipi_csi1 {
	unsigned char clock_inv:1;
	unsigned char strobe:1;
	bool lane_polarity[2];
	unsigned char data_lane;
	unsigned char clock_lane;
};


struct v4l2_fwnode_endpoint {
	struct fwnode_endpoint base;
	
	enum v4l2_mbus_type bus_type;
	struct {
		struct v4l2_fwnode_bus_parallel parallel;
		struct v4l2_fwnode_bus_mipi_csi1 mipi_csi1;
		struct v4l2_fwnode_bus_mipi_csi2 mipi_csi2;
	} bus;
	u64 *link_frequencies;
	unsigned int nr_of_link_frequencies;
};


#define V4L2_FWNODE_PROPERTY_UNSET   (-1U)


enum v4l2_fwnode_orientation {
	V4L2_FWNODE_ORIENTATION_FRONT,
	V4L2_FWNODE_ORIENTATION_BACK,
	V4L2_FWNODE_ORIENTATION_EXTERNAL
};


struct v4l2_fwnode_device_properties {
	enum v4l2_fwnode_orientation orientation;
	unsigned int rotation;
};


struct v4l2_fwnode_link {
	struct fwnode_handle *local_node;
	unsigned int local_port;
	unsigned int local_id;
	struct fwnode_handle *remote_node;
	unsigned int remote_port;
	unsigned int remote_id;
};


enum v4l2_connector_type {
	V4L2_CONN_UNKNOWN,
	V4L2_CONN_COMPOSITE,
	V4L2_CONN_SVIDEO,
};


struct v4l2_connector_link {
	struct list_head head;
	struct v4l2_fwnode_link fwnode_link;
};


struct v4l2_fwnode_connector_analog {
	v4l2_std_id sdtv_stds;
};


struct v4l2_fwnode_connector {
	const char *name;
	const char *label;
	enum v4l2_connector_type type;
	struct list_head links;
	unsigned int nr_of_links;

	union {
		struct v4l2_fwnode_connector_analog analog;
		
	} connector;
};


int v4l2_fwnode_endpoint_parse(struct fwnode_handle *fwnode,
			       struct v4l2_fwnode_endpoint *vep);


void v4l2_fwnode_endpoint_free(struct v4l2_fwnode_endpoint *vep);


int v4l2_fwnode_endpoint_alloc_parse(struct fwnode_handle *fwnode,
				     struct v4l2_fwnode_endpoint *vep);


int v4l2_fwnode_parse_link(struct fwnode_handle *fwnode,
			   struct v4l2_fwnode_link *link);


void v4l2_fwnode_put_link(struct v4l2_fwnode_link *link);


void v4l2_fwnode_connector_free(struct v4l2_fwnode_connector *connector);


int v4l2_fwnode_connector_parse(struct fwnode_handle *fwnode,
				struct v4l2_fwnode_connector *connector);


int v4l2_fwnode_connector_add_link(struct fwnode_handle *fwnode,
				   struct v4l2_fwnode_connector *connector);


int v4l2_fwnode_device_parse(struct device *dev,
			     struct v4l2_fwnode_device_properties *props);


typedef int (*parse_endpoint_func)(struct device *dev,
				  struct v4l2_fwnode_endpoint *vep,
				  struct v4l2_async_subdev *asd);


int
v4l2_async_notifier_parse_fwnode_endpoints(struct device *dev,
					   struct v4l2_async_notifier *notifier,
					   size_t asd_struct_size,
					   parse_endpoint_func parse_endpoint);


int
v4l2_async_notifier_parse_fwnode_endpoints_by_port(struct device *dev,
						   struct v4l2_async_notifier *notifier,
						   size_t asd_struct_size,
						   unsigned int port,
						   parse_endpoint_func parse_endpoint);


int v4l2_async_notifier_parse_fwnode_sensor_common(struct device *dev,
						   struct v4l2_async_notifier *notifier);




#define v4l2_connector_first_link(v4l2c)				       \
	list_first_entry_or_null(&(v4l2c)->links,			       \
				 struct v4l2_connector_link, head)


#define v4l2_connector_last_link(v4l2c)					       \
	list_last_entry(&(v4l2c)->links, struct v4l2_connector_link, head)

#endif 
