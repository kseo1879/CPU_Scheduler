/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_UACCESS_H__
#define __LINUX_UACCESS_H__

#include <linux/fault-inject-usercopy.h>
#include <linux/instrumented.h>
#include <linux/minmax.h>
#include <linux/sched.h>
#include <linux/thread_info.h>

#include <asm/uaccess.h>

#ifdef CONFIG_SET_FS

static inline mm_segment_t force_uaccess_begin(void)
{
	mm_segment_t fs = get_fs();

	set_fs(USER_DS);
	return fs;
}

static inline void force_uaccess_end(mm_segment_t oldfs)
{
	set_fs(oldfs);
}
#else 
typedef struct {
	
} mm_segment_t;

#ifndef TASK_SIZE_MAX
#define TASK_SIZE_MAX			TASK_SIZE
#endif

#define uaccess_kernel()		(false)
#define user_addr_max()			(TASK_SIZE_MAX)

static inline mm_segment_t force_uaccess_begin(void)
{
	return (mm_segment_t) { };
}

static inline void force_uaccess_end(mm_segment_t oldfs)
{
}
#endif 



static __always_inline __must_check unsigned long
__copy_from_user_inatomic(void *to, const void __user *from, unsigned long n)
{
	instrument_copy_from_user(to, from, n);
	check_object_size(to, n, false);
	return raw_copy_from_user(to, from, n);
}

static __always_inline __must_check unsigned long
__copy_from_user(void *to, const void __user *from, unsigned long n)
{
	might_fault();
	if (should_fail_usercopy())
		return n;
	instrument_copy_from_user(to, from, n);
	check_object_size(to, n, false);
	return raw_copy_from_user(to, from, n);
}


static __always_inline __must_check unsigned long
__copy_to_user_inatomic(void __user *to, const void *from, unsigned long n)
{
	if (should_fail_usercopy())
		return n;
	instrument_copy_to_user(to, from, n);
	check_object_size(from, n, true);
	return raw_copy_to_user(to, from, n);
}

static __always_inline __must_check unsigned long
__copy_to_user(void __user *to, const void *from, unsigned long n)
{
	might_fault();
	if (should_fail_usercopy())
		return n;
	instrument_copy_to_user(to, from, n);
	check_object_size(from, n, true);
	return raw_copy_to_user(to, from, n);
}

#ifdef INLINE_COPY_FROM_USER
static inline __must_check unsigned long
_copy_from_user(void *to, const void __user *from, unsigned long n)
{
	unsigned long res = n;
	might_fault();
	if (!should_fail_usercopy() && likely(access_ok(from, n))) {
		instrument_copy_from_user(to, from, n);
		res = raw_copy_from_user(to, from, n);
	}
	if (unlikely(res))
		memset(to + (n - res), 0, res);
	return res;
}
#else
extern __must_check unsigned long
_copy_from_user(void *, const void __user *, unsigned long);
#endif

#ifdef INLINE_COPY_TO_USER
static inline __must_check unsigned long
_copy_to_user(void __user *to, const void *from, unsigned long n)
{
	might_fault();
	if (should_fail_usercopy())
		return n;
	if (access_ok(to, n)) {
		instrument_copy_to_user(to, from, n);
		n = raw_copy_to_user(to, from, n);
	}
	return n;
}
#else
extern __must_check unsigned long
_copy_to_user(void __user *, const void *, unsigned long);
#endif

static __always_inline unsigned long __must_check
copy_from_user(void *to, const void __user *from, unsigned long n)
{
	if (likely(check_copy_size(to, n, false)))
		n = _copy_from_user(to, from, n);
	return n;
}

static __always_inline unsigned long __must_check
copy_to_user(void __user *to, const void *from, unsigned long n)
{
	if (likely(check_copy_size(from, n, true)))
		n = _copy_to_user(to, from, n);
	return n;
}
#ifdef CONFIG_COMPAT
static __always_inline unsigned long __must_check
copy_in_user(void __user *to, const void __user *from, unsigned long n)
{
	might_fault();
	if (access_ok(to, n) && access_ok(from, n))
		n = raw_copy_in_user(to, from, n);
	return n;
}
#endif

#ifndef copy_mc_to_kernel

static inline unsigned long __must_check
copy_mc_to_kernel(void *dst, const void *src, size_t cnt)
{
	memcpy(dst, src, cnt);
	return 0;
}
#endif

static __always_inline void pagefault_disabled_inc(void)
{
	current->pagefault_disabled++;
}

static __always_inline void pagefault_disabled_dec(void)
{
	current->pagefault_disabled--;
}


static inline void pagefault_disable(void)
{
	pagefault_disabled_inc();
	
	barrier();
}

static inline void pagefault_enable(void)
{
	
	barrier();
	pagefault_disabled_dec();
}


static inline bool pagefault_disabled(void)
{
	return current->pagefault_disabled != 0;
}


#define faulthandler_disabled() (pagefault_disabled() || in_atomic())

#ifndef ARCH_HAS_NOCACHE_UACCESS

static inline __must_check unsigned long
__copy_from_user_inatomic_nocache(void *to, const void __user *from,
				  unsigned long n)
{
	return __copy_from_user_inatomic(to, from, n);
}

#endif		

extern __must_check int check_zeroed_user(const void __user *from, size_t size);


static __always_inline __must_check int
copy_struct_from_user(void *dst, size_t ksize, const void __user *src,
		      size_t usize)
{
	size_t size = min(ksize, usize);
	size_t rest = max(ksize, usize) - size;

	
	if (usize < ksize) {
		memset(dst + size, 0, rest);
	} else if (usize > ksize) {
		int ret = check_zeroed_user(src + size, rest);
		if (ret <= 0)
			return ret ?: -E2BIG;
	}
	
	if (copy_from_user(dst, src, size))
		return -EFAULT;
	return 0;
}

bool copy_from_kernel_nofault_allowed(const void *unsafe_src, size_t size);

long copy_from_kernel_nofault(void *dst, const void *src, size_t size);
long notrace copy_to_kernel_nofault(void *dst, const void *src, size_t size);

long copy_from_user_nofault(void *dst, const void __user *src, size_t size);
long notrace copy_to_user_nofault(void __user *dst, const void *src,
		size_t size);

long strncpy_from_kernel_nofault(char *dst, const void *unsafe_addr,
		long count);

long strncpy_from_user_nofault(char *dst, const void __user *unsafe_addr,
		long count);
long strnlen_user_nofault(const void __user *unsafe_addr, long count);


#define get_kernel_nofault(val, ptr) ({				\
	const typeof(val) *__gk_ptr = (ptr);			\
	copy_from_kernel_nofault(&(val), __gk_ptr, sizeof(val));\
})

#ifndef user_access_begin
#define user_access_begin(ptr,len) access_ok(ptr, len)
#define user_access_end() do { } while (0)
#define unsafe_op_wrap(op, err) do { if (unlikely(op)) goto err; } while (0)
#define unsafe_get_user(x,p,e) unsafe_op_wrap(__get_user(x,p),e)
#define unsafe_put_user(x,p,e) unsafe_op_wrap(__put_user(x,p),e)
#define unsafe_copy_to_user(d,s,l,e) unsafe_op_wrap(__copy_to_user(d,s,l),e)
static inline unsigned long user_access_save(void) { return 0UL; }
static inline void user_access_restore(unsigned long flags) { }
#endif
#ifndef user_write_access_begin
#define user_write_access_begin user_access_begin
#define user_write_access_end user_access_end
#endif
#ifndef user_read_access_begin
#define user_read_access_begin user_access_begin
#define user_read_access_end user_access_end
#endif

#ifdef CONFIG_HARDENED_USERCOPY
void usercopy_warn(const char *name, const char *detail, bool to_user,
		   unsigned long offset, unsigned long len);
void __noreturn usercopy_abort(const char *name, const char *detail,
			       bool to_user, unsigned long offset,
			       unsigned long len);
#endif

#endif		
