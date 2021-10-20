/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_COMPILER_TYPES_H
#error "Please don't include <linux/compiler-clang.h> directly, include <linux/compiler.h> instead."
#endif

#define CLANG_VERSION (__clang_major__ * 10000	\
		     + __clang_minor__ * 100	\
		     + __clang_patchlevel__)

#if CLANG_VERSION < 100001
#ifndef __BPF_TRACING__
# error Sorry, your version of Clang is too old - please use 10.0.1 or newer.
#endif
#endif




#define __UNIQUE_ID(prefix) __PASTE(__PASTE(__UNIQUE_ID_, prefix), __COUNTER__)


#define KASAN_ABI_VERSION 5



#if __has_feature(address_sanitizer) || __has_feature(hwaddress_sanitizer)

#define __SANITIZE_ADDRESS__
#define __no_sanitize_address \
		__attribute__((no_sanitize("address", "hwaddress")))
#else
#define __no_sanitize_address
#endif

#if __has_feature(thread_sanitizer)

#define __SANITIZE_THREAD__
#define __no_sanitize_thread \
		__attribute__((no_sanitize("thread")))
#else
#define __no_sanitize_thread
#endif

#if defined(CONFIG_ARCH_USE_BUILTIN_BSWAP)
#define __HAVE_BUILTIN_BSWAP32__
#define __HAVE_BUILTIN_BSWAP64__
#define __HAVE_BUILTIN_BSWAP16__
#endif 

#if __has_feature(undefined_behavior_sanitizer)

#define __no_sanitize_undefined \
		__attribute__((no_sanitize("undefined")))
#else
#define __no_sanitize_undefined
#endif


#if __has_feature(coverage_sanitizer)
#define __no_sanitize_coverage __attribute__((no_sanitize("coverage")))
#else
#define __no_sanitize_coverage
#endif


#if __has_builtin(__builtin_mul_overflow) && \
    __has_builtin(__builtin_add_overflow) && \
    __has_builtin(__builtin_sub_overflow)
#define COMPILER_HAS_GENERIC_BUILTIN_OVERFLOW 1
#endif

#if __has_feature(shadow_call_stack)
# define __noscs	__attribute__((__no_sanitize__("shadow-call-stack")))
#endif
