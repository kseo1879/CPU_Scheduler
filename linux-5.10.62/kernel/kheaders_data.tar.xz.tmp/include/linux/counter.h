/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _COUNTER_H_
#define _COUNTER_H_

#include <linux/counter_enum.h>
#include <linux/device.h>
#include <linux/types.h>

enum counter_count_direction {
	COUNTER_COUNT_DIRECTION_FORWARD = 0,
	COUNTER_COUNT_DIRECTION_BACKWARD
};
extern const char *const counter_count_direction_str[2];

enum counter_count_mode {
	COUNTER_COUNT_MODE_NORMAL = 0,
	COUNTER_COUNT_MODE_RANGE_LIMIT,
	COUNTER_COUNT_MODE_NON_RECYCLE,
	COUNTER_COUNT_MODE_MODULO_N
};
extern const char *const counter_count_mode_str[4];

struct counter_device;
struct counter_signal;


struct counter_signal_ext {
	const char *name;
	ssize_t (*read)(struct counter_device *counter,
			struct counter_signal *signal, void *priv, char *buf);
	ssize_t (*write)(struct counter_device *counter,
			 struct counter_signal *signal, void *priv,
			 const char *buf, size_t len);
	void *priv;
};


struct counter_signal {
	int id;
	const char *name;

	const struct counter_signal_ext *ext;
	size_t num_ext;

	void *priv;
};


struct counter_signal_enum_ext {
	const char * const *items;
	size_t num_items;
	int (*get)(struct counter_device *counter,
		   struct counter_signal *signal, size_t *item);
	int (*set)(struct counter_device *counter,
		   struct counter_signal *signal, size_t item);
};


#define COUNTER_SIGNAL_ENUM(_name, _e) \
{ \
	.name = (_name), \
	.read = counter_signal_enum_read, \
	.write = counter_signal_enum_write, \
	.priv = (_e) \
}


#define COUNTER_SIGNAL_ENUM_AVAILABLE(_name, _e) \
{ \
	.name = (_name "_available"), \
	.read = counter_signal_enum_available_read, \
	.priv = (_e) \
}

enum counter_synapse_action {
	COUNTER_SYNAPSE_ACTION_NONE = 0,
	COUNTER_SYNAPSE_ACTION_RISING_EDGE,
	COUNTER_SYNAPSE_ACTION_FALLING_EDGE,
	COUNTER_SYNAPSE_ACTION_BOTH_EDGES
};


struct counter_synapse {
	size_t action;
	const enum counter_synapse_action *actions_list;
	size_t num_actions;

	struct counter_signal *signal;
};

struct counter_count;


struct counter_count_ext {
	const char *name;
	ssize_t (*read)(struct counter_device *counter,
			struct counter_count *count, void *priv, char *buf);
	ssize_t (*write)(struct counter_device *counter,
			 struct counter_count *count, void *priv,
			 const char *buf, size_t len);
	void *priv;
};

enum counter_count_function {
	COUNTER_COUNT_FUNCTION_INCREASE = 0,
	COUNTER_COUNT_FUNCTION_DECREASE,
	COUNTER_COUNT_FUNCTION_PULSE_DIRECTION,
	COUNTER_COUNT_FUNCTION_QUADRATURE_X1_A,
	COUNTER_COUNT_FUNCTION_QUADRATURE_X1_B,
	COUNTER_COUNT_FUNCTION_QUADRATURE_X2_A,
	COUNTER_COUNT_FUNCTION_QUADRATURE_X2_B,
	COUNTER_COUNT_FUNCTION_QUADRATURE_X4
};


struct counter_count {
	int id;
	const char *name;

	size_t function;
	const enum counter_count_function *functions_list;
	size_t num_functions;

	struct counter_synapse *synapses;
	size_t num_synapses;

	const struct counter_count_ext *ext;
	size_t num_ext;

	void *priv;
};


struct counter_count_enum_ext {
	const char * const *items;
	size_t num_items;
	int (*get)(struct counter_device *counter, struct counter_count *count,
		   size_t *item);
	int (*set)(struct counter_device *counter, struct counter_count *count,
		   size_t item);
};


#define COUNTER_COUNT_ENUM(_name, _e) \
{ \
	.name = (_name), \
	.read = counter_count_enum_read, \
	.write = counter_count_enum_write, \
	.priv = (_e) \
}


#define COUNTER_COUNT_ENUM_AVAILABLE(_name, _e) \
{ \
	.name = (_name "_available"), \
	.read = counter_count_enum_available_read, \
	.priv = (_e) \
}


struct counter_device_attr_group {
	struct attribute_group attr_group;
	struct list_head attr_list;
	size_t num_attr;
};


struct counter_device_state {
	int id;
	struct device dev;
	struct counter_device_attr_group *groups_list;
	size_t num_groups;
	const struct attribute_group **groups;
};

enum counter_signal_value {
	COUNTER_SIGNAL_LOW = 0,
	COUNTER_SIGNAL_HIGH
};


struct counter_ops {
	int (*signal_read)(struct counter_device *counter,
			   struct counter_signal *signal,
			   enum counter_signal_value *val);
	int (*count_read)(struct counter_device *counter,
			  struct counter_count *count, unsigned long *val);
	int (*count_write)(struct counter_device *counter,
			   struct counter_count *count, unsigned long val);
	int (*function_get)(struct counter_device *counter,
			    struct counter_count *count, size_t *function);
	int (*function_set)(struct counter_device *counter,
			    struct counter_count *count, size_t function);
	int (*action_get)(struct counter_device *counter,
			  struct counter_count *count,
			  struct counter_synapse *synapse, size_t *action);
	int (*action_set)(struct counter_device *counter,
			  struct counter_count *count,
			  struct counter_synapse *synapse, size_t action);
};


struct counter_device_ext {
	const char *name;
	ssize_t (*read)(struct counter_device *counter, void *priv, char *buf);
	ssize_t (*write)(struct counter_device *counter, void *priv,
			 const char *buf, size_t len);
	void *priv;
};


struct counter_device_enum_ext {
	const char * const *items;
	size_t num_items;
	int (*get)(struct counter_device *counter, size_t *item);
	int (*set)(struct counter_device *counter, size_t item);
};


#define COUNTER_DEVICE_ENUM(_name, _e) \
{ \
	.name = (_name), \
	.read = counter_device_enum_read, \
	.write = counter_device_enum_write, \
	.priv = (_e) \
}


#define COUNTER_DEVICE_ENUM_AVAILABLE(_name, _e) \
{ \
	.name = (_name "_available"), \
	.read = counter_device_enum_available_read, \
	.priv = (_e) \
}


struct counter_device {
	const char *name;
	struct device *parent;
	struct counter_device_state *device_state;

	const struct counter_ops *ops;

	struct counter_signal *signals;
	size_t num_signals;
	struct counter_count *counts;
	size_t num_counts;

	const struct counter_device_ext *ext;
	size_t num_ext;

	void *priv;
};

int counter_register(struct counter_device *const counter);
void counter_unregister(struct counter_device *const counter);
int devm_counter_register(struct device *dev,
			  struct counter_device *const counter);
void devm_counter_unregister(struct device *dev,
			     struct counter_device *const counter);

#endif 
