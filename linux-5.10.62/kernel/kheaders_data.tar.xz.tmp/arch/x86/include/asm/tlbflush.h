/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_X86_TLBFLUSH_H
#define _ASM_X86_TLBFLUSH_H

#include <linux/mm.h>
#include <linux/sched.h>

#include <asm/processor.h>
#include <asm/cpufeature.h>
#include <asm/special_insns.h>
#include <asm/smp.h>
#include <asm/invpcid.h>
#include <asm/pti.h>
#include <asm/processor-flags.h>

void __flush_tlb_all(void);

#define TLB_FLUSH_ALL	-1UL

void cr4_update_irqsoff(unsigned long set, unsigned long clear);
unsigned long cr4_read_shadow(void);


static inline void cr4_set_bits_irqsoff(unsigned long mask)
{
	cr4_update_irqsoff(mask, 0);
}


static inline void cr4_clear_bits_irqsoff(unsigned long mask)
{
	cr4_update_irqsoff(0, mask);
}


static inline void cr4_set_bits(unsigned long mask)
{
	unsigned long flags;

	local_irq_save(flags);
	cr4_set_bits_irqsoff(mask);
	local_irq_restore(flags);
}


static inline void cr4_clear_bits(unsigned long mask)
{
	unsigned long flags;

	local_irq_save(flags);
	cr4_clear_bits_irqsoff(mask);
	local_irq_restore(flags);
}

#ifndef MODULE

#define TLB_NR_DYN_ASIDS	6

struct tlb_context {
	u64 ctx_id;
	u64 tlb_gen;
};

struct tlb_state {
	
	struct mm_struct *loaded_mm;

#define LOADED_MM_SWITCHING ((struct mm_struct *)1UL)

	
	union {
		struct mm_struct	*last_user_mm;
		unsigned long		last_user_mm_ibpb;
	};

	u16 loaded_mm_asid;
	u16 next_asid;

	
	bool is_lazy;

	
	bool invalidate_other;

	
	unsigned short user_pcid_flush_mask;

	
	unsigned long cr4;

	
	struct tlb_context ctxs[TLB_NR_DYN_ASIDS];
};
DECLARE_PER_CPU_SHARED_ALIGNED(struct tlb_state, cpu_tlbstate);

bool nmi_uaccess_okay(void);
#define nmi_uaccess_okay nmi_uaccess_okay


static inline void cr4_init_shadow(void)
{
	this_cpu_write(cpu_tlbstate.cr4, __read_cr4());
}

extern unsigned long mmu_cr4_features;
extern u32 *trampoline_cr4_features;

extern void initialize_tlbstate_and_flush(void);


struct flush_tlb_info {
	
	struct mm_struct	*mm;
	unsigned long		start;
	unsigned long		end;
	u64			new_tlb_gen;
	unsigned int		stride_shift;
	bool			freed_tables;
};

void flush_tlb_local(void);
void flush_tlb_one_user(unsigned long addr);
void flush_tlb_one_kernel(unsigned long addr);
void flush_tlb_others(const struct cpumask *cpumask,
		      const struct flush_tlb_info *info);

#ifdef CONFIG_PARAVIRT
#include <asm/paravirt.h>
#endif

#define flush_tlb_mm(mm)						\
		flush_tlb_mm_range(mm, 0UL, TLB_FLUSH_ALL, 0UL, true)

#define flush_tlb_range(vma, start, end)				\
	flush_tlb_mm_range((vma)->vm_mm, start, end,			\
			   ((vma)->vm_flags & VM_HUGETLB)		\
				? huge_page_shift(hstate_vma(vma))	\
				: PAGE_SHIFT, false)

extern void flush_tlb_all(void);
extern void flush_tlb_mm_range(struct mm_struct *mm, unsigned long start,
				unsigned long end, unsigned int stride_shift,
				bool freed_tables);
extern void flush_tlb_kernel_range(unsigned long start, unsigned long end);

static inline void flush_tlb_page(struct vm_area_struct *vma, unsigned long a)
{
	flush_tlb_mm_range(vma->vm_mm, a, a + PAGE_SIZE, PAGE_SHIFT, false);
}

static inline u64 inc_mm_tlb_gen(struct mm_struct *mm)
{
	
	return atomic64_inc_return(&mm->context.tlb_gen);
}

static inline void arch_tlbbatch_add_mm(struct arch_tlbflush_unmap_batch *batch,
					struct mm_struct *mm)
{
	inc_mm_tlb_gen(mm);
	cpumask_or(&batch->cpumask, &batch->cpumask, mm_cpumask(mm));
}

extern void arch_tlbbatch_flush(struct arch_tlbflush_unmap_batch *batch);

#endif 

#endif 
