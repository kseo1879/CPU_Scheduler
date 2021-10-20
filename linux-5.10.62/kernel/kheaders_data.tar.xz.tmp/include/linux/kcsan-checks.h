/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _LINUX_KCSAN_CHECKS_H
#define _LINUX_KCSAN_CHECKS_H


#include <linux/compiler_attributes.h>
#include <linux/types.h>


#define KCSAN_ACCESS_WRITE	(1 << 0) 
#define KCSAN_ACCESS_COMPOUND	(1 << 1) 
#define KCSAN_ACCESS_ATOMIC	(1 << 2) 

#define KCSAN_ACCESS_ASSERT	(1 << 3) 
#define KCSAN_ACCESS_SCOPED	(1 << 4) 


#ifdef CONFIG_KCSAN

void __kcsan_check_access(const volatile void *ptr, size_t size, int type);


void kcsan_disable_current(void);


void kcsan_enable_current(void);
void kcsan_enable_current_nowarn(void); 


void kcsan_nestable_atomic_begin(void);


void kcsan_nestable_atomic_end(void);


void kcsan_flat_atomic_begin(void);


void kcsan_flat_atomic_end(void);


void kcsan_atomic_next(int n);


void kcsan_set_access_mask(unsigned long mask);


struct kcsan_scoped_access {
	struct list_head list;
	const volatile void *ptr;
	size_t size;
	int type;
};

#define __kcsan_cleanup_scoped                                                 \
	__maybe_unused __attribute__((__cleanup__(kcsan_end_scoped_access)))


struct kcsan_scoped_access *
kcsan_begin_scoped_access(const volatile void *ptr, size_t size, int type,
			  struct kcsan_scoped_access *sa);


void kcsan_end_scoped_access(struct kcsan_scoped_access *sa);


#else 

static inline void __kcsan_check_access(const volatile void *ptr, size_t size,
					int type) { }

static inline void kcsan_disable_current(void)		{ }
static inline void kcsan_enable_current(void)		{ }
static inline void kcsan_enable_current_nowarn(void)	{ }
static inline void kcsan_nestable_atomic_begin(void)	{ }
static inline void kcsan_nestable_atomic_end(void)	{ }
static inline void kcsan_flat_atomic_begin(void)	{ }
static inline void kcsan_flat_atomic_end(void)		{ }
static inline void kcsan_atomic_next(int n)		{ }
static inline void kcsan_set_access_mask(unsigned long mask) { }

struct kcsan_scoped_access { };
#define __kcsan_cleanup_scoped __maybe_unused
static inline struct kcsan_scoped_access *
kcsan_begin_scoped_access(const volatile void *ptr, size_t size, int type,
			  struct kcsan_scoped_access *sa) { return sa; }
static inline void kcsan_end_scoped_access(struct kcsan_scoped_access *sa) { }

#endif 

#ifdef __SANITIZE_THREAD__

#define kcsan_check_access __kcsan_check_access


#define __kcsan_disable_current kcsan_disable_current
#define __kcsan_enable_current kcsan_enable_current_nowarn
#else
static inline void kcsan_check_access(const volatile void *ptr, size_t size,
				      int type) { }
static inline void __kcsan_enable_current(void)  { }
static inline void __kcsan_disable_current(void) { }
#endif


#define __kcsan_check_read(ptr, size) __kcsan_check_access(ptr, size, 0)


#define __kcsan_check_write(ptr, size)                                         \
	__kcsan_check_access(ptr, size, KCSAN_ACCESS_WRITE)


#define __kcsan_check_read_write(ptr, size)                                    \
	__kcsan_check_access(ptr, size, KCSAN_ACCESS_COMPOUND | KCSAN_ACCESS_WRITE)


#define kcsan_check_read(ptr, size) kcsan_check_access(ptr, size, 0)


#define kcsan_check_write(ptr, size)                                           \
	kcsan_check_access(ptr, size, KCSAN_ACCESS_WRITE)


#define kcsan_check_read_write(ptr, size)                                      \
	kcsan_check_access(ptr, size, KCSAN_ACCESS_COMPOUND | KCSAN_ACCESS_WRITE)


#ifdef CONFIG_KCSAN_IGNORE_ATOMICS
#define kcsan_check_atomic_read(...)		do { } while (0)
#define kcsan_check_atomic_write(...)		do { } while (0)
#define kcsan_check_atomic_read_write(...)	do { } while (0)
#else
#define kcsan_check_atomic_read(ptr, size)                                     \
	kcsan_check_access(ptr, size, KCSAN_ACCESS_ATOMIC)
#define kcsan_check_atomic_write(ptr, size)                                    \
	kcsan_check_access(ptr, size, KCSAN_ACCESS_ATOMIC | KCSAN_ACCESS_WRITE)
#define kcsan_check_atomic_read_write(ptr, size)                               \
	kcsan_check_access(ptr, size, KCSAN_ACCESS_ATOMIC | KCSAN_ACCESS_WRITE | KCSAN_ACCESS_COMPOUND)
#endif


#define ASSERT_EXCLUSIVE_WRITER(var)                                           \
	__kcsan_check_access(&(var), sizeof(var), KCSAN_ACCESS_ASSERT)


#define __kcsan_scoped_name(c, suffix) __kcsan_scoped_##c##suffix
#define __ASSERT_EXCLUSIVE_SCOPED(var, type, id)                               \
	struct kcsan_scoped_access __kcsan_scoped_name(id, _)                  \
		__kcsan_cleanup_scoped;                                        \
	struct kcsan_scoped_access *__kcsan_scoped_name(id, _dummy_p)          \
		__maybe_unused = kcsan_begin_scoped_access(                    \
			&(var), sizeof(var), KCSAN_ACCESS_SCOPED | (type),     \
			&__kcsan_scoped_name(id, _))


#define ASSERT_EXCLUSIVE_WRITER_SCOPED(var)                                    \
	__ASSERT_EXCLUSIVE_SCOPED(var, KCSAN_ACCESS_ASSERT, __COUNTER__)


#define ASSERT_EXCLUSIVE_ACCESS(var)                                           \
	__kcsan_check_access(&(var), sizeof(var), KCSAN_ACCESS_WRITE | KCSAN_ACCESS_ASSERT)


#define ASSERT_EXCLUSIVE_ACCESS_SCOPED(var)                                    \
	__ASSERT_EXCLUSIVE_SCOPED(var, KCSAN_ACCESS_WRITE | KCSAN_ACCESS_ASSERT, __COUNTER__)


#define ASSERT_EXCLUSIVE_BITS(var, mask)                                       \
	do {                                                                   \
		kcsan_set_access_mask(mask);                                   \
		__kcsan_check_access(&(var), sizeof(var), KCSAN_ACCESS_ASSERT);\
		kcsan_set_access_mask(0);                                      \
		kcsan_atomic_next(1);                                          \
	} while (0)

#endif 
