/* SPDX-License-Identifier: GPL-2.0 */



#ifndef _ASM_GENERIC_HYPERV_TLFS_H
#define _ASM_GENERIC_HYPERV_TLFS_H

#include <linux/types.h>
#include <linux/bits.h>
#include <linux/time64.h>


#define HV_HYP_PAGE_SHIFT      12
#define HV_HYP_PAGE_SIZE       BIT(HV_HYP_PAGE_SHIFT)
#define HV_HYP_PAGE_MASK       (~(HV_HYP_PAGE_SIZE - 1))






#define HV_MSR_VP_RUNTIME_AVAILABLE		BIT(0)

#define HV_MSR_TIME_REF_COUNT_AVAILABLE		BIT(1)

#define HV_MSR_SYNIC_AVAILABLE			BIT(2)

#define HV_MSR_SYNTIMER_AVAILABLE		BIT(3)

#define HV_MSR_APIC_ACCESS_AVAILABLE		BIT(4)

#define HV_MSR_HYPERCALL_AVAILABLE		BIT(5)

#define HV_MSR_VP_INDEX_AVAILABLE		BIT(6)

#define HV_MSR_RESET_AVAILABLE			BIT(7)

#define HV_MSR_STAT_PAGES_AVAILABLE		BIT(8)

#define HV_MSR_REFERENCE_TSC_AVAILABLE		BIT(9)

#define HV_MSR_GUEST_IDLE_AVAILABLE		BIT(10)

#define HV_ACCESS_FREQUENCY_MSRS		BIT(11)

#define HV_ACCESS_REENLIGHTENMENT		BIT(13)

#define HV_ACCESS_TSC_INVARIANT			BIT(15)


#define HV_CREATE_PARTITIONS			BIT(0)
#define HV_ACCESS_PARTITION_ID			BIT(1)
#define HV_ACCESS_MEMORY_POOL			BIT(2)
#define HV_ADJUST_MESSAGE_BUFFERS		BIT(3)
#define HV_POST_MESSAGES			BIT(4)
#define HV_SIGNAL_EVENTS			BIT(5)
#define HV_CREATE_PORT				BIT(6)
#define HV_CONNECT_PORT				BIT(7)
#define HV_ACCESS_STATS				BIT(8)
#define HV_DEBUGGING				BIT(11)
#define HV_CPU_POWER_MANAGEMENT			BIT(12)



struct ms_hyperv_tsc_page {
	volatile u32 tsc_sequence;
	u32 reserved1;
	volatile u64 tsc_scale;
	volatile s64 tsc_offset;
} __packed;



#define HV_LINUX_VENDOR_ID              0x8100


#define HV_CRASH_CTL_CRASH_NOTIFY_MSG		BIT_ULL(62)
#define HV_CRASH_CTL_CRASH_NOTIFY		BIT_ULL(63)


#define HVCALL_FLUSH_VIRTUAL_ADDRESS_SPACE	0x0002
#define HVCALL_FLUSH_VIRTUAL_ADDRESS_LIST	0x0003
#define HVCALL_NOTIFY_LONG_SPIN_WAIT		0x0008
#define HVCALL_SEND_IPI				0x000b
#define HVCALL_FLUSH_VIRTUAL_ADDRESS_SPACE_EX	0x0013
#define HVCALL_FLUSH_VIRTUAL_ADDRESS_LIST_EX	0x0014
#define HVCALL_SEND_IPI_EX			0x0015
#define HVCALL_GET_VP_REGISTERS			0x0050
#define HVCALL_SET_VP_REGISTERS			0x0051
#define HVCALL_POST_MESSAGE			0x005c
#define HVCALL_SIGNAL_EVENT			0x005d
#define HVCALL_POST_DEBUG_DATA			0x0069
#define HVCALL_RETRIEVE_DEBUG_DATA		0x006a
#define HVCALL_RESET_DEBUG_SESSION		0x006b
#define HVCALL_RETARGET_INTERRUPT		0x007e
#define HVCALL_FLUSH_GUEST_PHYSICAL_ADDRESS_SPACE 0x00af
#define HVCALL_FLUSH_GUEST_PHYSICAL_ADDRESS_LIST 0x00b0

#define HV_FLUSH_ALL_PROCESSORS			BIT(0)
#define HV_FLUSH_ALL_VIRTUAL_ADDRESS_SPACES	BIT(1)
#define HV_FLUSH_NON_GLOBAL_MAPPINGS_ONLY	BIT(2)
#define HV_FLUSH_USE_EXTENDED_RANGE_FORMAT	BIT(3)

enum HV_GENERIC_SET_FORMAT {
	HV_GENERIC_SET_SPARSE_4K,
	HV_GENERIC_SET_ALL,
};

#define HV_PARTITION_ID_SELF		((u64)-1)
#define HV_VP_INDEX_SELF		((u32)-2)

#define HV_HYPERCALL_RESULT_MASK	GENMASK_ULL(15, 0)
#define HV_HYPERCALL_FAST_BIT		BIT(16)
#define HV_HYPERCALL_VARHEAD_OFFSET	17
#define HV_HYPERCALL_REP_COMP_OFFSET	32
#define HV_HYPERCALL_REP_COMP_1		BIT_ULL(32)
#define HV_HYPERCALL_REP_COMP_MASK	GENMASK_ULL(43, 32)
#define HV_HYPERCALL_REP_START_OFFSET	48
#define HV_HYPERCALL_REP_START_MASK	GENMASK_ULL(59, 48)


#define HV_STATUS_SUCCESS			0
#define HV_STATUS_INVALID_HYPERCALL_CODE	2
#define HV_STATUS_INVALID_HYPERCALL_INPUT	3
#define HV_STATUS_INVALID_ALIGNMENT		4
#define HV_STATUS_INVALID_PARAMETER		5
#define HV_STATUS_OPERATION_DENIED		8
#define HV_STATUS_INSUFFICIENT_MEMORY		11
#define HV_STATUS_INVALID_PORT_ID		17
#define HV_STATUS_INVALID_CONNECTION_ID		18
#define HV_STATUS_INSUFFICIENT_BUFFERS		19


#define HV_CLOCK_HZ (NSEC_PER_SEC/100)


#define HV_SYNIC_SINT_COUNT		(16)

#define HV_SYNIC_VERSION_1		(0x1)

#define HV_SYNIC_FIRST_VALID_VECTOR	(16)

#define HV_SYNIC_CONTROL_ENABLE		(1ULL << 0)
#define HV_SYNIC_SIMP_ENABLE		(1ULL << 0)
#define HV_SYNIC_SIEFP_ENABLE		(1ULL << 0)
#define HV_SYNIC_SINT_MASKED		(1ULL << 16)
#define HV_SYNIC_SINT_AUTO_EOI		(1ULL << 17)
#define HV_SYNIC_SINT_VECTOR_MASK	(0xFF)

#define HV_SYNIC_STIMER_COUNT		(4)


#define HV_MESSAGE_SIZE			(256)
#define HV_MESSAGE_PAYLOAD_BYTE_COUNT	(240)
#define HV_MESSAGE_PAYLOAD_QWORD_COUNT	(30)


union hv_message_flags {
	__u8 asu8;
	struct {
		__u8 msg_pending:1;
		__u8 reserved:7;
	} __packed;
};


union hv_port_id {
	__u32 asu32;
	struct {
		__u32 id:24;
		__u32 reserved:8;
	} __packed u;
};


struct hv_message_header {
	__u32 message_type;
	__u8 payload_size;
	union hv_message_flags message_flags;
	__u8 reserved[2];
	union {
		__u64 sender;
		union hv_port_id port;
	};
} __packed;


struct hv_message {
	struct hv_message_header header;
	union {
		__u64 payload[HV_MESSAGE_PAYLOAD_QWORD_COUNT];
	} u;
} __packed;


struct hv_message_page {
	struct hv_message sint_message[HV_SYNIC_SINT_COUNT];
} __packed;


struct hv_timer_message_payload {
	__u32 timer_index;
	__u32 reserved;
	__u64 expiration_time;	
	__u64 delivery_time;	
} __packed;



#define HV_EVENT_FLAGS_COUNT		(256 * 8)
#define HV_EVENT_FLAGS_LONG_COUNT	(256 / sizeof(unsigned long))


union hv_stimer_config {
	u64 as_uint64;
	struct {
		u64 enable:1;
		u64 periodic:1;
		u64 lazy:1;
		u64 auto_enable:1;
		u64 apic_vector:8;
		u64 direct_mode:1;
		u64 reserved_z0:3;
		u64 sintx:4;
		u64 reserved_z1:44;
	} __packed;
};



union hv_synic_event_flags {
	unsigned long flags[HV_EVENT_FLAGS_LONG_COUNT];
};


union hv_synic_scontrol {
	u64 as_uint64;
	struct {
		u64 enable:1;
		u64 reserved:63;
	} __packed;
};


union hv_synic_sint {
	u64 as_uint64;
	struct {
		u64 vector:8;
		u64 reserved1:8;
		u64 masked:1;
		u64 auto_eoi:1;
		u64 polling:1;
		u64 reserved2:45;
	} __packed;
};


union hv_synic_simp {
	u64 as_uint64;
	struct {
		u64 simp_enabled:1;
		u64 preserved:11;
		u64 base_simp_gpa:52;
	} __packed;
};


union hv_synic_siefp {
	u64 as_uint64;
	struct {
		u64 siefp_enabled:1;
		u64 preserved:11;
		u64 base_siefp_gpa:52;
	} __packed;
};

struct hv_vpset {
	u64 format;
	u64 valid_bank_mask;
	u64 bank_contents[];
} __packed;


struct hv_send_ipi {
	u32 vector;
	u32 reserved;
	u64 cpu_mask;
} __packed;


struct hv_send_ipi_ex {
	u32 vector;
	u32 reserved;
	struct hv_vpset vp_set;
} __packed;


struct hv_guest_mapping_flush {
	u64 address_space;
	u64 flags;
} __packed;


#define HV_MAX_FLUSH_PAGES (2048)


union hv_gpa_page_range {
	u64 address_space;
	struct {
		u64 additional_pages:11;
		u64 largepage:1;
		u64 basepfn:52;
	} page;
};


#define HV_MAX_FLUSH_REP_COUNT ((HV_HYP_PAGE_SIZE - 2 * sizeof(u64)) /	\
				sizeof(union hv_gpa_page_range))

struct hv_guest_mapping_flush_list {
	u64 address_space;
	u64 flags;
	union hv_gpa_page_range gpa_list[HV_MAX_FLUSH_REP_COUNT];
};


struct hv_tlb_flush {
	u64 address_space;
	u64 flags;
	u64 processor_mask;
	u64 gva_list[];
} __packed;


struct hv_tlb_flush_ex {
	u64 address_space;
	u64 flags;
	struct hv_vpset hv_vp_set;
	u64 gva_list[];
} __packed;


union hv_msi_entry {
	u64 as_uint64;
	struct {
		u32 address;
		u32 data;
	} __packed;
};

struct hv_interrupt_entry {
	u32 source;			
	u32 reserved1;
	union hv_msi_entry msi_entry;
} __packed;


#define HV_DEVICE_INTERRUPT_TARGET_MULTICAST		1
#define HV_DEVICE_INTERRUPT_TARGET_PROCESSOR_SET	2

struct hv_device_interrupt_target {
	u32 vector;
	u32 flags;
	union {
		u64 vp_mask;
		struct hv_vpset vp_set;
	};
} __packed;

struct hv_retarget_device_interrupt {
	u64 partition_id;		
	u64 device_id;
	struct hv_interrupt_entry int_entry;
	u64 reserved2;
	struct hv_device_interrupt_target int_target;
} __packed __aligned(8);



struct hv_get_vp_registers_input {
	struct {
		u64 partitionid;
		u32 vpindex;
		u8  inputvtl;
		u8  padding[3];
	} header;
	struct input {
		u32 name0;
		u32 name1;
	} element[];
} __packed;



struct hv_get_vp_registers_output {
	union {
		struct {
			u32 a;
			u32 b;
			u32 c;
			u32 d;
		} as32 __packed;
		struct {
			u64 low;
			u64 high;
		} as64 __packed;
	};
};


struct hv_set_vp_registers_input {
	struct {
		u64 partitionid;
		u32 vpindex;
		u8  inputvtl;
		u8  padding[3];
	} header;
	struct {
		u32 name;
		u32 padding1;
		u64 padding2;
		u64 valuelow;
		u64 valuehigh;
	} element[];
} __packed;

#endif
