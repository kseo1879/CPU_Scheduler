/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __PSTORE_BLK_H_
#define __PSTORE_BLK_H_

#include <linux/types.h>
#include <linux/pstore.h>
#include <linux/pstore_zone.h>


typedef int (*pstore_blk_panic_write_op)(const char *buf, sector_t start_sect,
		sector_t sects);


struct pstore_blk_info {
	unsigned int major;
	unsigned int flags;
	pstore_blk_panic_write_op panic_write;

	
	dev_t devt;
	sector_t nr_sects;
	sector_t start_sect;
};

int  register_pstore_blk(struct pstore_blk_info *info);
void unregister_pstore_blk(unsigned int major);


struct pstore_device_info {
	unsigned long total_size;
	unsigned int flags;
	pstore_zone_read_op read;
	pstore_zone_write_op write;
	pstore_zone_erase_op erase;
	pstore_zone_write_op panic_write;
};

int  register_pstore_device(struct pstore_device_info *dev);
void unregister_pstore_device(struct pstore_device_info *dev);


struct pstore_blk_config {
	char device[80];
	enum kmsg_dump_reason max_reason;
	unsigned long kmsg_size;
	unsigned long pmsg_size;
	unsigned long console_size;
	unsigned long ftrace_size;
};


int pstore_blk_get_config(struct pstore_blk_config *info);

#endif
