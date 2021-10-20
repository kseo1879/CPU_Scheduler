/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _ASM_X86_SUSPEND_32_H
#define _ASM_X86_SUSPEND_32_H

#include <asm/desc.h>
#include <asm/fpu/api.h>


struct saved_context {
	
#ifdef CONFIG_X86_32_LAZY_GS
	u16 gs;
#endif
	unsigned long cr0, cr2, cr3, cr4;
	u64 misc_enable;
	bool misc_enable_saved;
	struct saved_msrs saved_msrs;
	struct desc_ptr gdt_desc;
	struct desc_ptr idt;
	u16 ldt;
	u16 tss;
	unsigned long tr;
	unsigned long safety;
	unsigned long return_address;
} __attribute__((packed));


extern char core_restore_code[];
extern char restore_registers[];

#endif 
