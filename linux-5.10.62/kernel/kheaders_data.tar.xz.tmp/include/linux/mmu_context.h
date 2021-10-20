/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_MMU_CONTEXT_H
#define _LINUX_MMU_CONTEXT_H

#include <asm/mmu_context.h>
#include <asm/mmu.h>


#ifndef switch_mm_irqs_off
# define switch_mm_irqs_off switch_mm
#endif

#ifndef leave_mm
static inline void leave_mm(int cpu) { }
#endif

#endif
