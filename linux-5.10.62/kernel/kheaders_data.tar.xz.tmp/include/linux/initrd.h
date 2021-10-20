/* SPDX-License-Identifier: GPL-2.0 */

#define INITRD_MINOR 250 


extern int rd_image_start;


extern unsigned long rd_size;


extern int initrd_below_start_ok;


extern unsigned long initrd_start, initrd_end;
extern void free_initrd_mem(unsigned long, unsigned long);

extern phys_addr_t phys_initrd_start;
extern unsigned long phys_initrd_size;

extern unsigned int real_root_dev;

extern char __initramfs_start[];
extern unsigned long __initramfs_size;

void console_on_rootfs(void);
