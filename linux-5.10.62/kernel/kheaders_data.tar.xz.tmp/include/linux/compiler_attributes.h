/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_COMPILER_ATTRIBUTES_H
#define __LINUX_COMPILER_ATTRIBUTES_H




#ifndef __has_attribute
# define __has_attribute(x) __GCC4_has_attribute_##x
# define __GCC4_has_attribute___assume_aligned__      (__GNUC_MINOR__ >= 9)
# define __GCC4_has_attribute___copy__                0
# define __GCC4_has_attribute___designated_init__     0
# define __GCC4_has_attribute___externally_visible__  1
# define __GCC4_has_attribute___no_caller_saved_registers__ 0
# define __GCC4_has_attribute___noclone__             1
# define __GCC4_has_attribute___nonstring__           0
# define __GCC4_has_attribute___no_sanitize_address__ (__GNUC_MINOR__ >= 8)
# define __GCC4_has_attribute___no_sanitize_undefined__ (__GNUC_MINOR__ >= 9)
# define __GCC4_has_attribute___fallthrough__         0
#endif


#define __alias(symbol)                 __attribute__((__alias__(#symbol)))


#define __aligned(x)                    __attribute__((__aligned__(x)))
#define __aligned_largest               __attribute__((__aligned__))


#define __always_inline                 inline __attribute__((__always_inline__))


#if __has_attribute(__assume_aligned__)
# define __assume_aligned(a, ...)       __attribute__((__assume_aligned__(a, ## __VA_ARGS__)))
#else
# define __assume_aligned(a, ...)
#endif


#define __cold                          __attribute__((__cold__))


#define __attribute_const__             __attribute__((__const__))


#if __has_attribute(__copy__)
# define __copy(symbol)                 __attribute__((__copy__(symbol)))
#else
# define __copy(symbol)
#endif


#define __deprecated


#if __has_attribute(__designated_init__)
# define __designated_init              __attribute__((__designated_init__))
#else
# define __designated_init
#endif


#if __has_attribute(__externally_visible__)
# define __visible                      __attribute__((__externally_visible__))
#else
# define __visible
#endif


#define __printf(a, b)                  __attribute__((__format__(printf, a, b)))
#define __scanf(a, b)                   __attribute__((__format__(scanf, a, b)))


#define __gnu_inline                    __attribute__((__gnu_inline__))


#define __malloc                        __attribute__((__malloc__))


#define __mode(x)                       __attribute__((__mode__(x)))


#if __has_attribute(__no_caller_saved_registers__)
# define __no_caller_saved_registers	__attribute__((__no_caller_saved_registers__))
#else
# define __no_caller_saved_registers
#endif


#if __has_attribute(__noclone__)
# define __noclone                      __attribute__((__noclone__))
#else
# define __noclone
#endif


#if __has_attribute(__fallthrough__)
# define fallthrough                    __attribute__((__fallthrough__))
#else
# define fallthrough                    do {} while (0)  
#endif


#define   noinline                      __attribute__((__noinline__))


#if __has_attribute(__nonstring__)
# define __nonstring                    __attribute__((__nonstring__))
#else
# define __nonstring
#endif


#define __noreturn                      __attribute__((__noreturn__))


#define __packed                        __attribute__((__packed__))


#define __pure                          __attribute__((__pure__))


#define __section(section)              __attribute__((__section__(section)))


#define __always_unused                 __attribute__((__unused__))
#define __maybe_unused                  __attribute__((__unused__))


#define __used                          __attribute__((__used__))


#define __weak                          __attribute__((__weak__))

#endif 
