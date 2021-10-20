/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _PMBUS_H_
#define _PMBUS_H_

#include <linux/bits.h>




#define PMBUS_SKIP_STATUS_CHECK	BIT(0)


#define PMBUS_WRITE_PROTECTED	BIT(1)

struct pmbus_platform_data {
	u32 flags;		

	
	int num_regulators;
	struct regulator_init_data *reg_init_data;
};

#endif 
