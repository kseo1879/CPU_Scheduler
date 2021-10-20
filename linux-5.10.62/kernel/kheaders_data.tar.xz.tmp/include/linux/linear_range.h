/* SPDX-License-Identifier: GPL-2.0 */


#ifndef LINEAR_RANGE_H
#define LINEAR_RANGE_H

#include <linux/types.h>


struct linear_range {
	unsigned int min;
	unsigned int min_sel;
	unsigned int max_sel;
	unsigned int step;
};

unsigned int linear_range_values_in_range(const struct linear_range *r);
unsigned int linear_range_values_in_range_array(const struct linear_range *r,
						int ranges);
unsigned int linear_range_get_max_value(const struct linear_range *r);

int linear_range_get_value(const struct linear_range *r, unsigned int selector,
			   unsigned int *val);
int linear_range_get_value_array(const struct linear_range *r, int ranges,
				 unsigned int selector, unsigned int *val);
int linear_range_get_selector_low(const struct linear_range *r,
				  unsigned int val, unsigned int *selector,
				  bool *found);
int linear_range_get_selector_high(const struct linear_range *r,
				   unsigned int val, unsigned int *selector,
				   bool *found);
int linear_range_get_selector_low_array(const struct linear_range *r,
					int ranges, unsigned int val,
					unsigned int *selector, bool *found);

#endif
