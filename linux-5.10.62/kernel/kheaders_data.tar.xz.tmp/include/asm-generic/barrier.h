/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __ASM_GENERIC_BARRIER_H
#define __ASM_GENERIC_BARRIER_H

#ifndef __ASSEMBLY__

#include <linux/compiler.h>
#include <asm/rwonce.h>

#ifndef nop
#define nop()	asm volatile ("nop")
#endif



#ifndef mb
#define mb()	barrier()
#endif

#ifndef rmb
#define rmb()	mb()
#endif

#ifndef wmb
#define wmb()	mb()
#endif

#ifndef dma_rmb
#define dma_rmb()	rmb()
#endif

#ifndef dma_wmb
#define dma_wmb()	wmb()
#endif

#ifndef __smp_mb
#define __smp_mb()	mb()
#endif

#ifndef __smp_rmb
#define __smp_rmb()	rmb()
#endif

#ifndef __smp_wmb
#define __smp_wmb()	wmb()
#endif

#ifdef CONFIG_SMP

#ifndef smp_mb
#define smp_mb()	__smp_mb()
#endif

#ifndef smp_rmb
#define smp_rmb()	__smp_rmb()
#endif

#ifndef smp_wmb
#define smp_wmb()	__smp_wmb()
#endif

#else	

#ifndef smp_mb
#define smp_mb()	barrier()
#endif

#ifndef smp_rmb
#define smp_rmb()	barrier()
#endif

#ifndef smp_wmb
#define smp_wmb()	barrier()
#endif

#endif	

#ifndef __smp_store_mb
#define __smp_store_mb(var, value)  do { WRITE_ONCE(var, value); __smp_mb(); } while (0)
#endif

#ifndef __smp_mb__before_atomic
#define __smp_mb__before_atomic()	__smp_mb()
#endif

#ifndef __smp_mb__after_atomic
#define __smp_mb__after_atomic()	__smp_mb()
#endif

#ifndef __smp_store_release
#define __smp_store_release(p, v)					\
do {									\
	compiletime_assert_atomic_type(*p);				\
	__smp_mb();							\
	WRITE_ONCE(*p, v);						\
} while (0)
#endif

#ifndef __smp_load_acquire
#define __smp_load_acquire(p)						\
({									\
	__unqual_scalar_typeof(*p) ___p1 = READ_ONCE(*p);		\
	compiletime_assert_atomic_type(*p);				\
	__smp_mb();							\
	(typeof(*p))___p1;						\
})
#endif

#ifdef CONFIG_SMP

#ifndef smp_store_mb
#define smp_store_mb(var, value)  __smp_store_mb(var, value)
#endif

#ifndef smp_mb__before_atomic
#define smp_mb__before_atomic()	__smp_mb__before_atomic()
#endif

#ifndef smp_mb__after_atomic
#define smp_mb__after_atomic()	__smp_mb__after_atomic()
#endif

#ifndef smp_store_release
#define smp_store_release(p, v) __smp_store_release(p, v)
#endif

#ifndef smp_load_acquire
#define smp_load_acquire(p) __smp_load_acquire(p)
#endif

#else	

#ifndef smp_store_mb
#define smp_store_mb(var, value)  do { WRITE_ONCE(var, value); barrier(); } while (0)
#endif

#ifndef smp_mb__before_atomic
#define smp_mb__before_atomic()	barrier()
#endif

#ifndef smp_mb__after_atomic
#define smp_mb__after_atomic()	barrier()
#endif

#ifndef smp_store_release
#define smp_store_release(p, v)						\
do {									\
	compiletime_assert_atomic_type(*p);				\
	barrier();							\
	WRITE_ONCE(*p, v);						\
} while (0)
#endif

#ifndef smp_load_acquire
#define smp_load_acquire(p)						\
({									\
	__unqual_scalar_typeof(*p) ___p1 = READ_ONCE(*p);		\
	compiletime_assert_atomic_type(*p);				\
	barrier();							\
	(typeof(*p))___p1;						\
})
#endif

#endif	


#define virt_mb() __smp_mb()
#define virt_rmb() __smp_rmb()
#define virt_wmb() __smp_wmb()
#define virt_store_mb(var, value) __smp_store_mb(var, value)
#define virt_mb__before_atomic() __smp_mb__before_atomic()
#define virt_mb__after_atomic()	__smp_mb__after_atomic()
#define virt_store_release(p, v) __smp_store_release(p, v)
#define virt_load_acquire(p) __smp_load_acquire(p)


#ifndef smp_acquire__after_ctrl_dep
#define smp_acquire__after_ctrl_dep()		smp_rmb()
#endif


#ifndef smp_cond_load_relaxed
#define smp_cond_load_relaxed(ptr, cond_expr) ({		\
	typeof(ptr) __PTR = (ptr);				\
	__unqual_scalar_typeof(*ptr) VAL;			\
	for (;;) {						\
		VAL = READ_ONCE(*__PTR);			\
		if (cond_expr)					\
			break;					\
		cpu_relax();					\
	}							\
	(typeof(*ptr))VAL;					\
})
#endif


#ifndef smp_cond_load_acquire
#define smp_cond_load_acquire(ptr, cond_expr) ({		\
	__unqual_scalar_typeof(*ptr) _val;			\
	_val = smp_cond_load_relaxed(ptr, cond_expr);		\
	smp_acquire__after_ctrl_dep();				\
	(typeof(*ptr))_val;					\
})
#endif


#ifndef pmem_wmb
#define pmem_wmb()	wmb()
#endif

#endif 
#endif 
