/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_FAULT_INJECT_USERCOPY_H__
#define __LINUX_FAULT_INJECT_USERCOPY_H__



#include <linux/types.h>

#ifdef CONFIG_FAULT_INJECTION_USERCOPY

bool should_fail_usercopy(void);

#else

static inline bool should_fail_usercopy(void) { return false; }

#endif 

#endif 
