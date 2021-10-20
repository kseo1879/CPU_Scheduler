/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _LINUX_STACKDEPOT_H
#define _LINUX_STACKDEPOT_H

typedef u32 depot_stack_handle_t;

depot_stack_handle_t stack_depot_save(unsigned long *entries,
				      unsigned int nr_entries, gfp_t gfp_flags);

unsigned int stack_depot_fetch(depot_stack_handle_t handle,
			       unsigned long **entries);

unsigned int filter_irq_stacks(unsigned long *entries, unsigned int nr_entries);

#endif
