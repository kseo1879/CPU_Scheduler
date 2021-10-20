/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _LINUX_SCMI_PROTOCOL_H
#define _LINUX_SCMI_PROTOCOL_H

#include <linux/device.h>
#include <linux/notifier.h>
#include <linux/types.h>

#define SCMI_MAX_STR_SIZE	16
#define SCMI_MAX_NUM_RATES	16


struct scmi_revision_info {
	u16 major_ver;
	u16 minor_ver;
	u8 num_protocols;
	u8 num_agents;
	u32 impl_ver;
	char vendor_id[SCMI_MAX_STR_SIZE];
	char sub_vendor_id[SCMI_MAX_STR_SIZE];
};

struct scmi_clock_info {
	char name[SCMI_MAX_STR_SIZE];
	bool rate_discrete;
	union {
		struct {
			int num_rates;
			u64 rates[SCMI_MAX_NUM_RATES];
		} list;
		struct {
			u64 min_rate;
			u64 max_rate;
			u64 step_size;
		} range;
	};
};

struct scmi_handle;


struct scmi_clk_ops {
	int (*count_get)(const struct scmi_handle *handle);

	const struct scmi_clock_info *(*info_get)
		(const struct scmi_handle *handle, u32 clk_id);
	int (*rate_get)(const struct scmi_handle *handle, u32 clk_id,
			u64 *rate);
	int (*rate_set)(const struct scmi_handle *handle, u32 clk_id,
			u64 rate);
	int (*enable)(const struct scmi_handle *handle, u32 clk_id);
	int (*disable)(const struct scmi_handle *handle, u32 clk_id);
};


struct scmi_perf_ops {
	int (*limits_set)(const struct scmi_handle *handle, u32 domain,
			  u32 max_perf, u32 min_perf);
	int (*limits_get)(const struct scmi_handle *handle, u32 domain,
			  u32 *max_perf, u32 *min_perf);
	int (*level_set)(const struct scmi_handle *handle, u32 domain,
			 u32 level, bool poll);
	int (*level_get)(const struct scmi_handle *handle, u32 domain,
			 u32 *level, bool poll);
	int (*device_domain_id)(struct device *dev);
	int (*transition_latency_get)(const struct scmi_handle *handle,
				      struct device *dev);
	int (*device_opps_add)(const struct scmi_handle *handle,
			       struct device *dev);
	int (*freq_set)(const struct scmi_handle *handle, u32 domain,
			unsigned long rate, bool poll);
	int (*freq_get)(const struct scmi_handle *handle, u32 domain,
			unsigned long *rate, bool poll);
	int (*est_power_get)(const struct scmi_handle *handle, u32 domain,
			     unsigned long *rate, unsigned long *power);
	bool (*fast_switch_possible)(const struct scmi_handle *handle,
				     struct device *dev);
};


struct scmi_power_ops {
	int (*num_domains_get)(const struct scmi_handle *handle);
	char *(*name_get)(const struct scmi_handle *handle, u32 domain);
#define SCMI_POWER_STATE_TYPE_SHIFT	30
#define SCMI_POWER_STATE_ID_MASK	(BIT(28) - 1)
#define SCMI_POWER_STATE_PARAM(type, id) \
	((((type) & BIT(0)) << SCMI_POWER_STATE_TYPE_SHIFT) | \
		((id) & SCMI_POWER_STATE_ID_MASK))
#define SCMI_POWER_STATE_GENERIC_ON	SCMI_POWER_STATE_PARAM(0, 0)
#define SCMI_POWER_STATE_GENERIC_OFF	SCMI_POWER_STATE_PARAM(1, 0)
	int (*state_set)(const struct scmi_handle *handle, u32 domain,
			 u32 state);
	int (*state_get)(const struct scmi_handle *handle, u32 domain,
			 u32 *state);
};

struct scmi_sensor_info {
	u32 id;
	u8 type;
	s8 scale;
	u8 num_trip_points;
	bool async;
	char name[SCMI_MAX_STR_SIZE];
};


enum scmi_sensor_class {
	NONE = 0x0,
	TEMPERATURE_C = 0x2,
	VOLTAGE = 0x5,
	CURRENT = 0x6,
	POWER = 0x7,
	ENERGY = 0x8,
};


struct scmi_sensor_ops {
	int (*count_get)(const struct scmi_handle *handle);
	const struct scmi_sensor_info *(*info_get)
		(const struct scmi_handle *handle, u32 sensor_id);
	int (*trip_point_config)(const struct scmi_handle *handle,
				 u32 sensor_id, u8 trip_id, u64 trip_value);
	int (*reading_get)(const struct scmi_handle *handle, u32 sensor_id,
			   u64 *value);
};


struct scmi_reset_ops {
	int (*num_domains_get)(const struct scmi_handle *handle);
	char *(*name_get)(const struct scmi_handle *handle, u32 domain);
	int (*latency_get)(const struct scmi_handle *handle, u32 domain);
	int (*reset)(const struct scmi_handle *handle, u32 domain);
	int (*assert)(const struct scmi_handle *handle, u32 domain);
	int (*deassert)(const struct scmi_handle *handle, u32 domain);
};


struct scmi_notify_ops {
	int (*register_event_notifier)(const struct scmi_handle *handle,
				       u8 proto_id, u8 evt_id, u32 *src_id,
				       struct notifier_block *nb);
	int (*unregister_event_notifier)(const struct scmi_handle *handle,
					 u8 proto_id, u8 evt_id, u32 *src_id,
					 struct notifier_block *nb);
};


struct scmi_handle {
	struct device *dev;
	struct scmi_revision_info *version;
	const struct scmi_perf_ops *perf_ops;
	const struct scmi_clk_ops *clk_ops;
	const struct scmi_power_ops *power_ops;
	const struct scmi_sensor_ops *sensor_ops;
	const struct scmi_reset_ops *reset_ops;
	const struct scmi_notify_ops *notify_ops;
	
	void *perf_priv;
	void *clk_priv;
	void *power_priv;
	void *sensor_priv;
	void *reset_priv;
	void *notify_priv;
	void *system_priv;
};

enum scmi_std_protocol {
	SCMI_PROTOCOL_BASE = 0x10,
	SCMI_PROTOCOL_POWER = 0x11,
	SCMI_PROTOCOL_SYSTEM = 0x12,
	SCMI_PROTOCOL_PERF = 0x13,
	SCMI_PROTOCOL_CLOCK = 0x14,
	SCMI_PROTOCOL_SENSOR = 0x15,
	SCMI_PROTOCOL_RESET = 0x16,
};

enum scmi_system_events {
	SCMI_SYSTEM_SHUTDOWN,
	SCMI_SYSTEM_COLDRESET,
	SCMI_SYSTEM_WARMRESET,
	SCMI_SYSTEM_POWERUP,
	SCMI_SYSTEM_SUSPEND,
	SCMI_SYSTEM_MAX
};

struct scmi_device {
	u32 id;
	u8 protocol_id;
	const char *name;
	struct device dev;
	struct scmi_handle *handle;
};

#define to_scmi_dev(d) container_of(d, struct scmi_device, dev)

struct scmi_device *
scmi_device_create(struct device_node *np, struct device *parent, int protocol,
		   const char *name);
void scmi_device_destroy(struct scmi_device *scmi_dev);

struct scmi_device_id {
	u8 protocol_id;
	const char *name;
};

struct scmi_driver {
	const char *name;
	int (*probe)(struct scmi_device *sdev);
	void (*remove)(struct scmi_device *sdev);
	const struct scmi_device_id *id_table;

	struct device_driver driver;
};

#define to_scmi_driver(d) container_of(d, struct scmi_driver, driver)

#if IS_REACHABLE(CONFIG_ARM_SCMI_PROTOCOL)
int scmi_driver_register(struct scmi_driver *driver,
			 struct module *owner, const char *mod_name);
void scmi_driver_unregister(struct scmi_driver *driver);
#else
static inline int
scmi_driver_register(struct scmi_driver *driver, struct module *owner,
		     const char *mod_name)
{
	return -EINVAL;
}

static inline void scmi_driver_unregister(struct scmi_driver *driver) {}
#endif 

#define scmi_register(driver) \
	scmi_driver_register(driver, THIS_MODULE, KBUILD_MODNAME)
#define scmi_unregister(driver) \
	scmi_driver_unregister(driver)


#define module_scmi_driver(__scmi_driver)	\
	module_driver(__scmi_driver, scmi_register, scmi_unregister)

typedef int (*scmi_prot_init_fn_t)(struct scmi_handle *);
int scmi_protocol_register(int protocol_id, scmi_prot_init_fn_t fn);
void scmi_protocol_unregister(int protocol_id);


enum scmi_notification_events {
	SCMI_EVENT_POWER_STATE_CHANGED = 0x0,
	SCMI_EVENT_PERFORMANCE_LIMITS_CHANGED = 0x0,
	SCMI_EVENT_PERFORMANCE_LEVEL_CHANGED = 0x1,
	SCMI_EVENT_SENSOR_TRIP_POINT_EVENT = 0x0,
	SCMI_EVENT_RESET_ISSUED = 0x0,
	SCMI_EVENT_BASE_ERROR_EVENT = 0x0,
	SCMI_EVENT_SYSTEM_POWER_STATE_NOTIFIER = 0x0,
};

struct scmi_power_state_changed_report {
	ktime_t		timestamp;
	unsigned int	agent_id;
	unsigned int	domain_id;
	unsigned int	power_state;
};

struct scmi_system_power_state_notifier_report {
	ktime_t		timestamp;
	unsigned int	agent_id;
	unsigned int	flags;
	unsigned int	system_state;
};

struct scmi_perf_limits_report {
	ktime_t		timestamp;
	unsigned int	agent_id;
	unsigned int	domain_id;
	unsigned int	range_max;
	unsigned int	range_min;
};

struct scmi_perf_level_report {
	ktime_t		timestamp;
	unsigned int	agent_id;
	unsigned int	domain_id;
	unsigned int	performance_level;
};

struct scmi_sensor_trip_point_report {
	ktime_t		timestamp;
	unsigned int	agent_id;
	unsigned int	sensor_id;
	unsigned int	trip_point_desc;
};

struct scmi_reset_issued_report {
	ktime_t		timestamp;
	unsigned int	agent_id;
	unsigned int	domain_id;
	unsigned int	reset_state;
};

struct scmi_base_error_report {
	ktime_t			timestamp;
	unsigned int		agent_id;
	bool			fatal;
	unsigned int		cmd_count;
	unsigned long long	reports[];
};

#endif 
