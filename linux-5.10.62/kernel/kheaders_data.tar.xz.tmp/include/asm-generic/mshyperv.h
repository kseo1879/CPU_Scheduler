/* SPDX-License-Identifier: GPL-2.0 */



#ifndef _ASM_GENERIC_MSHYPERV_H
#define _ASM_GENERIC_MSHYPERV_H

#include <linux/types.h>
#include <linux/atomic.h>
#include <linux/bitops.h>
#include <linux/cpumask.h>
#include <asm/ptrace.h>
#include <asm/hyperv-tlfs.h>

struct ms_hyperv_info {
	u32 features;
	u32 misc_features;
	u32 hints;
	u32 nested_features;
	u32 max_vp_index;
	u32 max_lp_index;
};
extern struct ms_hyperv_info ms_hyperv;

extern u64 hv_do_hypercall(u64 control, void *inputaddr, void *outputaddr);
extern u64 hv_do_fast_hypercall8(u16 control, u64 input8);



static inline  __u64 generate_guest_id(__u64 d_info1, __u64 kernel_version,
				       __u64 d_info2)
{
	__u64 guest_id = 0;

	guest_id = (((__u64)HV_LINUX_VENDOR_ID) << 48);
	guest_id |= (d_info1 << 48);
	guest_id |= (kernel_version << 16);
	guest_id |= d_info2;

	return guest_id;
}



static inline void vmbus_signal_eom(struct hv_message *msg, u32 old_msg_type)
{
	
	if (cmpxchg(&msg->header.message_type, old_msg_type,
		    HVMSG_NONE) != old_msg_type)
		return;

	
	if (msg->header.message_flags.msg_pending) {
		
		hv_signal_eom();
	}
}

int hv_setup_vmbus_irq(int irq, void (*handler)(void));
void hv_remove_vmbus_irq(void);
void hv_enable_vmbus_irq(void);
void hv_disable_vmbus_irq(void);

void hv_setup_kexec_handler(void (*handler)(void));
void hv_remove_kexec_handler(void);
void hv_setup_crash_handler(void (*handler)(struct pt_regs *regs));
void hv_remove_crash_handler(void);

extern int vmbus_interrupt;

#if IS_ENABLED(CONFIG_HYPERV)

extern u32 *hv_vp_index;
extern u32 hv_max_vp_index;


#define VP_INVAL	U32_MAX


static inline int hv_cpu_number_to_vp_number(int cpu_number)
{
	return hv_vp_index[cpu_number];
}

static inline int cpumask_to_vpset(struct hv_vpset *vpset,
				    const struct cpumask *cpus)
{
	int cpu, vcpu, vcpu_bank, vcpu_offset, nr_bank = 1;

	
	if (hv_max_vp_index / 64 >= 64)
		return 0;

	
	for (vcpu_bank = 0; vcpu_bank <= hv_max_vp_index / 64; vcpu_bank++)
		vpset->bank_contents[vcpu_bank] = 0;

	
	for_each_cpu(cpu, cpus) {
		vcpu = hv_cpu_number_to_vp_number(cpu);
		if (vcpu == VP_INVAL)
			return -1;
		vcpu_bank = vcpu / 64;
		vcpu_offset = vcpu % 64;
		__set_bit(vcpu_offset, (unsigned long *)
			  &vpset->bank_contents[vcpu_bank]);
		if (vcpu_bank >= nr_bank)
			nr_bank = vcpu_bank + 1;
	}
	vpset->valid_bank_mask = GENMASK_ULL(nr_bank - 1, 0);
	return nr_bank;
}

void hyperv_report_panic(struct pt_regs *regs, long err, bool in_die);
void hyperv_report_panic_msg(phys_addr_t pa, size_t size);
bool hv_is_hyperv_initialized(void);
bool hv_is_hibernation_supported(void);
void hyperv_cleanup(void);
#else 
static inline bool hv_is_hyperv_initialized(void) { return false; }
static inline bool hv_is_hibernation_supported(void) { return false; }
static inline void hyperv_cleanup(void) {}
#endif 

#if IS_ENABLED(CONFIG_HYPERV)
extern int hv_setup_stimer0_irq(int *irq, int *vector, void (*handler)(void));
extern void hv_remove_stimer0_irq(int irq);
#endif

#endif
