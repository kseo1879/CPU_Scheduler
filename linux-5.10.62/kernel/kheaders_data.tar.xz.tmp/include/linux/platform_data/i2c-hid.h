

#ifndef __LINUX_I2C_HID_H
#define __LINUX_I2C_HID_H

#include <linux/regulator/consumer.h>
#include <linux/types.h>


struct i2c_hid_platform_data {
	u16 hid_descriptor_address;
	struct regulator_bulk_data supplies[2];
	int post_power_delay_ms;
};

#endif 
