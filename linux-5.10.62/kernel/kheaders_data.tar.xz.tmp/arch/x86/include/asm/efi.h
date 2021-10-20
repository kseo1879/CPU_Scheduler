/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_X86_EFI_H
#define _ASM_X86_EFI_H

#include <asm/fpu/api.h>
#include <asm/processor-flags.h>
#include <asm/tlb.h>
#include <asm/nospec-branch.h>
#include <asm/mmu_context.h>
#include <linux/build_bug.h>
#include <linux/kernel.h>
#include <linux/pgtable.h>

extern unsigned long efi_fw_vendor, efi_config_table;



#define EFI32_LOADER_SIGNATURE	"EL32"
#define EFI64_LOADER_SIGNATURE	"EL64"

#define ARCH_EFI_IRQ_FLAGS_MASK	X86_EFLAGS_IF



#define __efi_nargs(...) __efi_nargs_(__VA_ARGS__)
#define __efi_nargs_(...) __efi_nargs__(0, ##__VA_ARGS__,	\
	__efi_arg_sentinel(7), __efi_arg_sentinel(6),		\
	__efi_arg_sentinel(5), __efi_arg_sentinel(4),		\
	__efi_arg_sentinel(3), __efi_arg_sentinel(2),		\
	__efi_arg_sentinel(1), __efi_arg_sentinel(0))
#define __efi_nargs__(_0, _1, _2, _3, _4, _5, _6, _7, n, ...)	\
	__take_second_arg(n,					\
		({ BUILD_BUG_ON_MSG(1, "__efi_nargs limit exceeded"); 8; }))
#define __efi_arg_sentinel(n) , n



#define __efi_nargs_check(f, n, ...)					\
	__efi_nargs_check_(f, __efi_nargs(__VA_ARGS__), n)
#define __efi_nargs_check_(f, p, n) __efi_nargs_check__(f, p, n)
#define __efi_nargs_check__(f, p, n) ({					\
	BUILD_BUG_ON_MSG(						\
		(p) > (n),						\
		#f " called with too many arguments (" #p ">" #n ")");	\
})

#ifdef CONFIG_X86_32
#define arch_efi_call_virt_setup()					\
({									\
	kernel_fpu_begin();						\
	firmware_restrict_branch_speculation_start();			\
})

#define arch_efi_call_virt_teardown()					\
({									\
	firmware_restrict_branch_speculation_end();			\
	kernel_fpu_end();						\
})

#define arch_efi_call_virt(p, f, args...)	p->f(args)

#else 

#define EFI_LOADER_SIGNATURE	"EL64"

extern asmlinkage u64 __efi_call(void *fp, ...);

#define efi_call(...) ({						\
	__efi_nargs_check(efi_call, 7, __VA_ARGS__);			\
	__efi_call(__VA_ARGS__);					\
})


struct efi_scratch {
	u64			phys_stack;
	struct mm_struct	*prev_mm;
} __packed;

#define arch_efi_call_virt_setup()					\
({									\
	efi_sync_low_kernel_mappings();					\
	kernel_fpu_begin();						\
	firmware_restrict_branch_speculation_start();			\
	efi_switch_mm(&efi_mm);						\
})

#define arch_efi_call_virt(p, f, args...)				\
	efi_call((void *)p->f, args)					\

#define arch_efi_call_virt_teardown()					\
({									\
	efi_switch_mm(efi_scratch.prev_mm);				\
	firmware_restrict_branch_speculation_end();			\
	kernel_fpu_end();						\
})

#ifdef CONFIG_KASAN

#undef memcpy
#undef memset
#undef memmove
#endif

#endif 

extern struct efi_scratch efi_scratch;
extern int __init efi_memblock_x86_reserve_range(void);
extern void __init efi_print_memmap(void);
extern void __init efi_map_region(efi_memory_desc_t *md);
extern void __init efi_map_region_fixed(efi_memory_desc_t *md);
extern void efi_sync_low_kernel_mappings(void);
extern int __init efi_alloc_page_tables(void);
extern int __init efi_setup_page_tables(unsigned long pa_memmap, unsigned num_pages);
extern void __init efi_runtime_update_mappings(void);
extern void __init efi_dump_pagetable(void);
extern void __init efi_apply_memmap_quirks(void);
extern int __init efi_reuse_config(u64 tables, int nr_tables);
extern void efi_delete_dummy_variable(void);
extern void efi_switch_mm(struct mm_struct *mm);
extern void efi_recover_from_page_fault(unsigned long phys_addr);
extern void efi_free_boot_services(void);


struct efi_setup_data {
	u64 fw_vendor;
	u64 __unused;
	u64 tables;
	u64 smbios;
	u64 reserved[8];
};

extern u64 efi_setup;

#ifdef CONFIG_EFI
extern efi_status_t __efi64_thunk(u32, ...);

#define efi64_thunk(...) ({						\
	__efi_nargs_check(efi64_thunk, 6, __VA_ARGS__);			\
	__efi64_thunk(__VA_ARGS__);					\
})

static inline bool efi_is_mixed(void)
{
	if (!IS_ENABLED(CONFIG_EFI_MIXED))
		return false;
	return IS_ENABLED(CONFIG_X86_64) && !efi_enabled(EFI_64BIT);
}

static inline bool efi_runtime_supported(void)
{
	if (IS_ENABLED(CONFIG_X86_64) == efi_enabled(EFI_64BIT))
		return true;

	return IS_ENABLED(CONFIG_EFI_MIXED);
}

extern void parse_efi_setup(u64 phys_addr, u32 data_len);

extern void efifb_setup_from_dmi(struct screen_info *si, const char *opt);

extern void efi_thunk_runtime_setup(void);
efi_status_t efi_set_virtual_address_map(unsigned long memory_map_size,
					 unsigned long descriptor_size,
					 u32 descriptor_version,
					 efi_memory_desc_t *virtual_map,
					 unsigned long systab_phys);



#ifdef CONFIG_EFI_MIXED

#define ARCH_HAS_EFISTUB_WRAPPERS

static inline bool efi_is_64bit(void)
{
	extern const bool efi_is64;

	return efi_is64;
}

static inline bool efi_is_native(void)
{
	if (!IS_ENABLED(CONFIG_X86_64))
		return true;
	return efi_is_64bit();
}

#define efi_mixed_mode_cast(attr)					\
	__builtin_choose_expr(						\
		__builtin_types_compatible_p(u32, __typeof__(attr)),	\
			(unsigned long)(attr), (attr))

#define efi_table_attr(inst, attr)					\
	(efi_is_native()						\
		? inst->attr						\
		: (__typeof__(inst->attr))				\
			efi_mixed_mode_cast(inst->mixed_mode.attr))



static inline void *efi64_zero_upper(void *p)
{
	((u32 *)p)[1] = 0;
	return p;
}

static inline u32 efi64_convert_status(efi_status_t status)
{
	return (u32)(status | (u64)status >> 32);
}

#define __efi64_argmap_free_pages(addr, size)				\
	((addr), 0, (size))

#define __efi64_argmap_get_memory_map(mm_size, mm, key, size, ver)	\
	((mm_size), (mm), efi64_zero_upper(key), efi64_zero_upper(size), (ver))

#define __efi64_argmap_allocate_pool(type, size, buffer)		\
	((type), (size), efi64_zero_upper(buffer))

#define __efi64_argmap_create_event(type, tpl, f, c, event)		\
	((type), (tpl), (f), (c), efi64_zero_upper(event))

#define __efi64_argmap_set_timer(event, type, time)			\
	((event), (type), lower_32_bits(time), upper_32_bits(time))

#define __efi64_argmap_wait_for_event(num, event, index)		\
	((num), (event), efi64_zero_upper(index))

#define __efi64_argmap_handle_protocol(handle, protocol, interface)	\
	((handle), (protocol), efi64_zero_upper(interface))

#define __efi64_argmap_locate_protocol(protocol, reg, interface)	\
	((protocol), (reg), efi64_zero_upper(interface))

#define __efi64_argmap_locate_device_path(protocol, path, handle)	\
	((protocol), (path), efi64_zero_upper(handle))

#define __efi64_argmap_exit(handle, status, size, data)			\
	((handle), efi64_convert_status(status), (size), (data))


#define __efi64_argmap_get_location(protocol, seg, bus, dev, func)	\
	((protocol), efi64_zero_upper(seg), efi64_zero_upper(bus),	\
	 efi64_zero_upper(dev), efi64_zero_upper(func))


#define __efi64_argmap_load_file(protocol, path, policy, bufsize, buf)	\
	((protocol), (path), (policy), efi64_zero_upper(bufsize), (buf))


#define __efi64_argmap_query_mode(gop, mode, size, info)		\
	((gop), (mode), efi64_zero_upper(size), efi64_zero_upper(info))



#define __efi64_thunk_map(inst, func, ...)				\
	efi64_thunk(inst->mixed_mode.func,				\
		__efi64_argmap(__efi64_argmap_ ## func(__VA_ARGS__),	\
			       (__VA_ARGS__)))

#define __efi64_argmap(mapped, args)					\
	__PASTE(__efi64_argmap__, __efi_nargs(__efi_eat mapped))(mapped, args)
#define __efi64_argmap__0(mapped, args) __efi_eval mapped
#define __efi64_argmap__1(mapped, args) __efi_eval args

#define __efi_eat(...)
#define __efi_eval(...) __VA_ARGS__



#define efi_call_proto(inst, func, ...)					\
	(efi_is_native()						\
		? inst->func(inst, ##__VA_ARGS__)			\
		: __efi64_thunk_map(inst, func, inst, ##__VA_ARGS__))

#define efi_bs_call(func, ...)						\
	(efi_is_native()						\
		? efi_system_table->boottime->func(__VA_ARGS__)		\
		: __efi64_thunk_map(efi_table_attr(efi_system_table,	\
						   boottime),		\
				    func, __VA_ARGS__))

#define efi_rt_call(func, ...)						\
	(efi_is_native()						\
		? efi_system_table->runtime->func(__VA_ARGS__)		\
		: __efi64_thunk_map(efi_table_attr(efi_system_table,	\
						   runtime),		\
				    func, __VA_ARGS__))

#else 

static inline bool efi_is_64bit(void)
{
	return IS_ENABLED(CONFIG_X86_64);
}

#endif 

extern bool efi_reboot_required(void);
extern bool efi_is_table_address(unsigned long phys_addr);

extern void efi_find_mirror(void);
extern void efi_reserve_boot_services(void);
#else
static inline void parse_efi_setup(u64 phys_addr, u32 data_len) {}
static inline bool efi_reboot_required(void)
{
	return false;
}
static inline  bool efi_is_table_address(unsigned long phys_addr)
{
	return false;
}
static inline void efi_find_mirror(void)
{
}
static inline void efi_reserve_boot_services(void)
{
}
#endif 

#ifdef CONFIG_EFI_FAKE_MEMMAP
extern void __init efi_fake_memmap_early(void);
#else
static inline void efi_fake_memmap_early(void)
{
}
#endif

#endif 
