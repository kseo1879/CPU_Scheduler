/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _ASM_X86_FPU_API_H
#define _ASM_X86_FPU_API_H
#include <linux/bottom_half.h>




#define KFPU_387	_BITUL(0)	
#define KFPU_MXCSR	_BITUL(1)	

extern void kernel_fpu_begin_mask(unsigned int kfpu_mask);
extern void kernel_fpu_end(void);
extern bool irq_fpu_usable(void);
extern void fpregs_mark_activate(void);


static inline void kernel_fpu_begin(void)
{
	kernel_fpu_begin_mask(KFPU_387 | KFPU_MXCSR);
}


static inline void fpregs_lock(void)
{
	preempt_disable();
	local_bh_disable();
}

static inline void fpregs_unlock(void)
{
	local_bh_enable();
	preempt_enable();
}

#ifdef CONFIG_X86_DEBUG_FPU
extern void fpregs_assert_state_consistent(void);
#else
static inline void fpregs_assert_state_consistent(void) { }
#endif


extern void switch_fpu_return(void);


extern int cpu_has_xfeatures(u64 xfeatures_mask, const char **feature_name);


#define PASID_DISABLED	0

static inline void update_pasid(void) { }

#endif 
