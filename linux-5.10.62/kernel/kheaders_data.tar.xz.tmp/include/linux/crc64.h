/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _LINUX_CRC64_H
#define _LINUX_CRC64_H

#include <linux/types.h>

u64 __pure crc64_be(u64 crc, const void *p, size_t len);
#endif 
