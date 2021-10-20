/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _ASM_X86_HIGHMEM_H
#define _ASM_X86_HIGHMEM_H

#ifdef __KERNEL__

#include <linux/interrupt.h>
#include <linux/threads.h>
#include <asm/kmap_types.h>
#include <asm/tlbflush.h>
#include <asm/paravirt.h>
#include <asm/fixmap.h>


extern unsigned long highstart_pfn, highend_pfn;



#define LAST_PKMAP_MASK (LAST_PKMAP-1)
#define PKMAP_NR(virt)  ((virt-PKMAP_BASE) >> PAGE_SHIFT)
#define PKMAP_ADDR(nr)  (PKMAP_BASE + ((nr) << PAGE_SHIFT))

void *kmap_atomic_pfn(unsigned long pfn);
void *kmap_atomic_prot_pfn(unsigned long pfn, pgprot_t prot);

#define flush_cache_kmaps()	do { } while (0)

extern void add_highpages_with_active_regions(int nid, unsigned long start_pfn,
					unsigned long end_pfn);

#endif 

#endif 
