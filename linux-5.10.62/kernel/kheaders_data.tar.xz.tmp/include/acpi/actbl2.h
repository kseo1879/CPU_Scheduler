/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0 */


#ifndef __ACTBL2_H__
#define __ACTBL2_H__




#define ACPI_SIG_IORT           "IORT"	
#define ACPI_SIG_IVRS           "IVRS"	
#define ACPI_SIG_LPIT           "LPIT"	
#define ACPI_SIG_MADT           "APIC"	
#define ACPI_SIG_MCFG           "MCFG"	
#define ACPI_SIG_MCHI           "MCHI"	
#define ACPI_SIG_MPST           "MPST"	
#define ACPI_SIG_MSCT           "MSCT"	
#define ACPI_SIG_MSDM           "MSDM"	
#define ACPI_SIG_MTMR           "MTMR"	
#define ACPI_SIG_NFIT           "NFIT"	
#define ACPI_SIG_PCCT           "PCCT"	
#define ACPI_SIG_PDTT           "PDTT"	
#define ACPI_SIG_PMTT           "PMTT"	
#define ACPI_SIG_PPTT           "PPTT"	
#define ACPI_SIG_RASF           "RASF"	
#define ACPI_SIG_SBST           "SBST"	
#define ACPI_SIG_SDEI           "SDEI"	
#define ACPI_SIG_SDEV           "SDEV"	
#define ACPI_SIG_NHLT           "NHLT"	


#pragma pack(1)





struct acpi_table_iort {
	struct acpi_table_header header;
	u32 node_count;
	u32 node_offset;
	u32 reserved;
};


struct acpi_iort_node {
	u8 type;
	u16 length;
	u8 revision;
	u32 reserved;
	u32 mapping_count;
	u32 mapping_offset;
	char node_data[1];
};



enum acpi_iort_node_type {
	ACPI_IORT_NODE_ITS_GROUP = 0x00,
	ACPI_IORT_NODE_NAMED_COMPONENT = 0x01,
	ACPI_IORT_NODE_PCI_ROOT_COMPLEX = 0x02,
	ACPI_IORT_NODE_SMMU = 0x03,
	ACPI_IORT_NODE_SMMU_V3 = 0x04,
	ACPI_IORT_NODE_PMCG = 0x05
};

struct acpi_iort_id_mapping {
	u32 input_base;		
	u32 id_count;		
	u32 output_base;	
	u32 output_reference;	
	u32 flags;
};



#define ACPI_IORT_ID_SINGLE_MAPPING (1)

struct acpi_iort_memory_access {
	u32 cache_coherency;
	u8 hints;
	u16 reserved;
	u8 memory_flags;
};



#define ACPI_IORT_NODE_COHERENT         0x00000001	
#define ACPI_IORT_NODE_NOT_COHERENT     0x00000000	



#define ACPI_IORT_HT_TRANSIENT          (1)
#define ACPI_IORT_HT_WRITE              (1<<1)
#define ACPI_IORT_HT_READ               (1<<2)
#define ACPI_IORT_HT_OVERRIDE           (1<<3)



#define ACPI_IORT_MF_COHERENCY          (1)
#define ACPI_IORT_MF_ATTRIBUTES         (1<<1)


struct acpi_iort_its_group {
	u32 its_count;
	u32 identifiers[1];	
};

struct acpi_iort_named_component {
	u32 node_flags;
	u64 memory_properties;	
	u8 memory_address_limit;	
	char device_name[1];	
};



#define ACPI_IORT_NC_STALL_SUPPORTED    (1)
#define ACPI_IORT_NC_PASID_BITS         (31<<1)

struct acpi_iort_root_complex {
	u64 memory_properties;	
	u32 ats_attribute;
	u32 pci_segment_number;
	u8 memory_address_limit;	
	u8 reserved[3];		
};



#define ACPI_IORT_ATS_SUPPORTED         0x00000001	
#define ACPI_IORT_ATS_UNSUPPORTED       0x00000000	

struct acpi_iort_smmu {
	u64 base_address;	
	u64 span;		
	u32 model;
	u32 flags;
	u32 global_interrupt_offset;
	u32 context_interrupt_count;
	u32 context_interrupt_offset;
	u32 pmu_interrupt_count;
	u32 pmu_interrupt_offset;
	u64 interrupts[1];	
};



#define ACPI_IORT_SMMU_V1               0x00000000	
#define ACPI_IORT_SMMU_V2               0x00000001	
#define ACPI_IORT_SMMU_CORELINK_MMU400  0x00000002	
#define ACPI_IORT_SMMU_CORELINK_MMU500  0x00000003	
#define ACPI_IORT_SMMU_CORELINK_MMU401  0x00000004	
#define ACPI_IORT_SMMU_CAVIUM_THUNDERX  0x00000005	



#define ACPI_IORT_SMMU_DVM_SUPPORTED    (1)
#define ACPI_IORT_SMMU_COHERENT_WALK    (1<<1)



struct acpi_iort_smmu_gsi {
	u32 nsg_irpt;
	u32 nsg_irpt_flags;
	u32 nsg_cfg_irpt;
	u32 nsg_cfg_irpt_flags;
};

struct acpi_iort_smmu_v3 {
	u64 base_address;	
	u32 flags;
	u32 reserved;
	u64 vatos_address;
	u32 model;
	u32 event_gsiv;
	u32 pri_gsiv;
	u32 gerr_gsiv;
	u32 sync_gsiv;
	u32 pxm;
	u32 id_mapping_index;
};



#define ACPI_IORT_SMMU_V3_GENERIC           0x00000000	
#define ACPI_IORT_SMMU_V3_HISILICON_HI161X  0x00000001	
#define ACPI_IORT_SMMU_V3_CAVIUM_CN99XX     0x00000002	



#define ACPI_IORT_SMMU_V3_COHACC_OVERRIDE   (1)
#define ACPI_IORT_SMMU_V3_HTTU_OVERRIDE     (3<<1)
#define ACPI_IORT_SMMU_V3_PXM_VALID         (1<<3)

struct acpi_iort_pmcg {
	u64 page0_base_address;
	u32 overflow_gsiv;
	u32 node_reference;
	u64 page1_base_address;
};



struct acpi_table_ivrs {
	struct acpi_table_header header;	
	u32 info;		
	u64 reserved;
};



#define ACPI_IVRS_PHYSICAL_SIZE     0x00007F00	
#define ACPI_IVRS_VIRTUAL_SIZE      0x003F8000	
#define ACPI_IVRS_ATS_RESERVED      0x00400000	



struct acpi_ivrs_header {
	u8 type;		
	u8 flags;
	u16 length;		
	u16 device_id;		
};



enum acpi_ivrs_type {
	ACPI_IVRS_TYPE_HARDWARE1 = 0x10,
	ACPI_IVRS_TYPE_HARDWARE2 = 0x11,
	ACPI_IVRS_TYPE_MEMORY1 = 0x20,
	ACPI_IVRS_TYPE_MEMORY2 = 0x21,
	ACPI_IVRS_TYPE_MEMORY3 = 0x22
};



#define ACPI_IVHD_TT_ENABLE         (1)
#define ACPI_IVHD_PASS_PW           (1<<1)
#define ACPI_IVHD_RES_PASS_PW       (1<<2)
#define ACPI_IVHD_ISOC              (1<<3)
#define ACPI_IVHD_IOTLB             (1<<4)



#define ACPI_IVMD_UNITY             (1)
#define ACPI_IVMD_READ              (1<<1)
#define ACPI_IVMD_WRITE             (1<<2)
#define ACPI_IVMD_EXCLUSION_RANGE   (1<<3)





struct acpi_ivrs_hardware_10 {
	struct acpi_ivrs_header header;
	u16 capability_offset;	
	u64 base_address;	
	u16 pci_segment_group;
	u16 info;		
	u32 feature_reporting;
};



struct acpi_ivrs_hardware_11 {
	struct acpi_ivrs_header header;
	u16 capability_offset;	
	u64 base_address;	
	u16 pci_segment_group;
	u16 info;		
	u32 attributes;
	u64 efr_register_image;
	u64 reserved;
};



#define ACPI_IVHD_MSI_NUMBER_MASK   0x001F	
#define ACPI_IVHD_UNIT_ID_MASK      0x1F00	


struct acpi_ivrs_de_header {
	u8 type;
	u16 id;
	u8 data_setting;
};



#define ACPI_IVHD_ENTRY_LENGTH      0xC0



enum acpi_ivrs_device_entry_type {
	

	ACPI_IVRS_TYPE_PAD4 = 0,
	ACPI_IVRS_TYPE_ALL = 1,
	ACPI_IVRS_TYPE_SELECT = 2,
	ACPI_IVRS_TYPE_START = 3,
	ACPI_IVRS_TYPE_END = 4,

	

	ACPI_IVRS_TYPE_PAD8 = 64,
	ACPI_IVRS_TYPE_NOT_USED = 65,
	ACPI_IVRS_TYPE_ALIAS_SELECT = 66,	
	ACPI_IVRS_TYPE_ALIAS_START = 67,	
	ACPI_IVRS_TYPE_EXT_SELECT = 70,	
	ACPI_IVRS_TYPE_EXT_START = 71,	
	ACPI_IVRS_TYPE_SPECIAL = 72	
};



#define ACPI_IVHD_INIT_PASS         (1)
#define ACPI_IVHD_EINT_PASS         (1<<1)
#define ACPI_IVHD_NMI_PASS          (1<<2)
#define ACPI_IVHD_SYSTEM_MGMT       (3<<4)
#define ACPI_IVHD_LINT0_PASS        (1<<6)
#define ACPI_IVHD_LINT1_PASS        (1<<7)



struct acpi_ivrs_device4 {
	struct acpi_ivrs_de_header header;
};



struct acpi_ivrs_device8a {
	struct acpi_ivrs_de_header header;
	u8 reserved1;
	u16 used_id;
	u8 reserved2;
};



struct acpi_ivrs_device8b {
	struct acpi_ivrs_de_header header;
	u32 extended_data;
};



#define ACPI_IVHD_ATS_DISABLED      (1<<31)



struct acpi_ivrs_device8c {
	struct acpi_ivrs_de_header header;
	u8 handle;
	u16 used_id;
	u8 variety;
};



#define ACPI_IVHD_IOAPIC            1
#define ACPI_IVHD_HPET              2



struct acpi_ivrs_memory {
	struct acpi_ivrs_header header;
	u16 aux_data;
	u64 reserved;
	u64 start_address;
	u64 memory_length;
};



struct acpi_table_lpit {
	struct acpi_table_header header;	
};



struct acpi_lpit_header {
	u32 type;		
	u32 length;		
	u16 unique_id;
	u16 reserved;
	u32 flags;
};



enum acpi_lpit_type {
	ACPI_LPIT_TYPE_NATIVE_CSTATE = 0x00,
	ACPI_LPIT_TYPE_RESERVED = 0x01	
};



#define ACPI_LPIT_STATE_DISABLED    (1)
#define ACPI_LPIT_NO_COUNTER        (1<<1)





struct acpi_lpit_native {
	struct acpi_lpit_header header;
	struct acpi_generic_address entry_trigger;
	u32 residency;
	u32 latency;
	struct acpi_generic_address residency_counter;
	u64 counter_frequency;
};



struct acpi_table_madt {
	struct acpi_table_header header;	
	u32 address;		
	u32 flags;
};



#define ACPI_MADT_PCAT_COMPAT       (1)	



#define ACPI_MADT_DUAL_PIC          1
#define ACPI_MADT_MULTIPLE_APIC     0



enum acpi_madt_type {
	ACPI_MADT_TYPE_LOCAL_APIC = 0,
	ACPI_MADT_TYPE_IO_APIC = 1,
	ACPI_MADT_TYPE_INTERRUPT_OVERRIDE = 2,
	ACPI_MADT_TYPE_NMI_SOURCE = 3,
	ACPI_MADT_TYPE_LOCAL_APIC_NMI = 4,
	ACPI_MADT_TYPE_LOCAL_APIC_OVERRIDE = 5,
	ACPI_MADT_TYPE_IO_SAPIC = 6,
	ACPI_MADT_TYPE_LOCAL_SAPIC = 7,
	ACPI_MADT_TYPE_INTERRUPT_SOURCE = 8,
	ACPI_MADT_TYPE_LOCAL_X2APIC = 9,
	ACPI_MADT_TYPE_LOCAL_X2APIC_NMI = 10,
	ACPI_MADT_TYPE_GENERIC_INTERRUPT = 11,
	ACPI_MADT_TYPE_GENERIC_DISTRIBUTOR = 12,
	ACPI_MADT_TYPE_GENERIC_MSI_FRAME = 13,
	ACPI_MADT_TYPE_GENERIC_REDISTRIBUTOR = 14,
	ACPI_MADT_TYPE_GENERIC_TRANSLATOR = 15,
	ACPI_MADT_TYPE_RESERVED = 16	
};





struct acpi_madt_local_apic {
	struct acpi_subtable_header header;
	u8 processor_id;	
	u8 id;			
	u32 lapic_flags;
};



struct acpi_madt_io_apic {
	struct acpi_subtable_header header;
	u8 id;			
	u8 reserved;		
	u32 address;		
	u32 global_irq_base;	
};



struct acpi_madt_interrupt_override {
	struct acpi_subtable_header header;
	u8 bus;			
	u8 source_irq;		
	u32 global_irq;		
	u16 inti_flags;
};



struct acpi_madt_nmi_source {
	struct acpi_subtable_header header;
	u16 inti_flags;
	u32 global_irq;		
};



struct acpi_madt_local_apic_nmi {
	struct acpi_subtable_header header;
	u8 processor_id;	
	u16 inti_flags;
	u8 lint;		
};



struct acpi_madt_local_apic_override {
	struct acpi_subtable_header header;
	u16 reserved;		
	u64 address;		
};



struct acpi_madt_io_sapic {
	struct acpi_subtable_header header;
	u8 id;			
	u8 reserved;		
	u32 global_irq_base;	
	u64 address;		
};



struct acpi_madt_local_sapic {
	struct acpi_subtable_header header;
	u8 processor_id;	
	u8 id;			
	u8 eid;			
	u8 reserved[3];		
	u32 lapic_flags;
	u32 uid;		
	char uid_string[1];	
};



struct acpi_madt_interrupt_source {
	struct acpi_subtable_header header;
	u16 inti_flags;
	u8 type;		
	u8 id;			
	u8 eid;			
	u8 io_sapic_vector;	
	u32 global_irq;		
	u32 flags;		
};



#define ACPI_MADT_CPEI_OVERRIDE     (1)



struct acpi_madt_local_x2apic {
	struct acpi_subtable_header header;
	u16 reserved;		
	u32 local_apic_id;	
	u32 lapic_flags;
	u32 uid;		
};



struct acpi_madt_local_x2apic_nmi {
	struct acpi_subtable_header header;
	u16 inti_flags;
	u32 uid;		
	u8 lint;		
	u8 reserved[3];		
};



struct acpi_madt_generic_interrupt {
	struct acpi_subtable_header header;
	u16 reserved;		
	u32 cpu_interface_number;
	u32 uid;
	u32 flags;
	u32 parking_version;
	u32 performance_interrupt;
	u64 parked_address;
	u64 base_address;
	u64 gicv_base_address;
	u64 gich_base_address;
	u32 vgic_interrupt;
	u64 gicr_base_address;
	u64 arm_mpidr;
	u8 efficiency_class;
	u8 reserved2[1];
	u16 spe_interrupt;	
};




#define ACPI_MADT_PERFORMANCE_IRQ_MODE  (1<<1)	
#define ACPI_MADT_VGIC_IRQ_MODE         (1<<2)	



struct acpi_madt_generic_distributor {
	struct acpi_subtable_header header;
	u16 reserved;		
	u32 gic_id;
	u64 base_address;
	u32 global_irq_base;
	u8 version;
	u8 reserved2[3];	
};



enum acpi_madt_gic_version {
	ACPI_MADT_GIC_VERSION_NONE = 0,
	ACPI_MADT_GIC_VERSION_V1 = 1,
	ACPI_MADT_GIC_VERSION_V2 = 2,
	ACPI_MADT_GIC_VERSION_V3 = 3,
	ACPI_MADT_GIC_VERSION_V4 = 4,
	ACPI_MADT_GIC_VERSION_RESERVED = 5	
};



struct acpi_madt_generic_msi_frame {
	struct acpi_subtable_header header;
	u16 reserved;		
	u32 msi_frame_id;
	u64 base_address;
	u32 flags;
	u16 spi_count;
	u16 spi_base;
};



#define ACPI_MADT_OVERRIDE_SPI_VALUES   (1)



struct acpi_madt_generic_redistributor {
	struct acpi_subtable_header header;
	u16 reserved;		
	u64 base_address;
	u32 length;
};



struct acpi_madt_generic_translator {
	struct acpi_subtable_header header;
	u16 reserved;		
	u32 translation_id;
	u64 base_address;
	u32 reserved2;
};





#define ACPI_MADT_ENABLED           (1)	



#define ACPI_MADT_POLARITY_MASK     (3)	
#define ACPI_MADT_TRIGGER_MASK      (3<<2)	



#define ACPI_MADT_POLARITY_CONFORMS       0
#define ACPI_MADT_POLARITY_ACTIVE_HIGH    1
#define ACPI_MADT_POLARITY_RESERVED       2
#define ACPI_MADT_POLARITY_ACTIVE_LOW     3

#define ACPI_MADT_TRIGGER_CONFORMS        (0)
#define ACPI_MADT_TRIGGER_EDGE            (1<<2)
#define ACPI_MADT_TRIGGER_RESERVED        (2<<2)
#define ACPI_MADT_TRIGGER_LEVEL           (3<<2)



struct acpi_table_mcfg {
	struct acpi_table_header header;	
	u8 reserved[8];
};



struct acpi_mcfg_allocation {
	u64 address;		
	u16 pci_segment;	
	u8 start_bus_number;	
	u8 end_bus_number;	
	u32 reserved;
};



struct acpi_table_mchi {
	struct acpi_table_header header;	
	u8 interface_type;
	u8 protocol;
	u64 protocol_data;
	u8 interrupt_type;
	u8 gpe;
	u8 pci_device_flag;
	u32 global_interrupt;
	struct acpi_generic_address control_register;
	u8 pci_segment;
	u8 pci_bus;
	u8 pci_device;
	u8 pci_function;
};



#define ACPI_MPST_CHANNEL_INFO \
	u8                              channel_id; \
	u8                              reserved1[3]; \
	u16                             power_node_count; \
	u16                             reserved2;



struct acpi_table_mpst {
	struct acpi_table_header header;	
	 ACPI_MPST_CHANNEL_INFO	
};



struct acpi_mpst_channel {
	ACPI_MPST_CHANNEL_INFO	
};



struct acpi_mpst_power_node {
	u8 flags;
	u8 reserved1;
	u16 node_id;
	u32 length;
	u64 range_address;
	u64 range_length;
	u32 num_power_states;
	u32 num_physical_components;
};



#define ACPI_MPST_ENABLED               1
#define ACPI_MPST_POWER_MANAGED         2
#define ACPI_MPST_HOT_PLUG_CAPABLE      4



struct acpi_mpst_power_state {
	u8 power_state;
	u8 info_index;
};



struct acpi_mpst_component {
	u16 component_id;
};



struct acpi_mpst_data_hdr {
	u16 characteristics_count;
	u16 reserved;
};

struct acpi_mpst_power_data {
	u8 structure_id;
	u8 flags;
	u16 reserved1;
	u32 average_power;
	u32 power_saving;
	u64 exit_latency;
	u64 reserved2;
};



#define ACPI_MPST_PRESERVE              1
#define ACPI_MPST_AUTOENTRY             2
#define ACPI_MPST_AUTOEXIT              4



struct acpi_mpst_shared {
	u32 signature;
	u16 pcc_command;
	u16 pcc_status;
	u32 command_register;
	u32 status_register;
	u32 power_state_id;
	u32 power_node_id;
	u64 energy_consumed;
	u64 average_power;
};



struct acpi_table_msct {
	struct acpi_table_header header;	
	u32 proximity_offset;	
	u32 max_proximity_domains;	
	u32 max_clock_domains;	
	u64 max_address;	
};



struct acpi_msct_proximity {
	u8 revision;
	u8 length;
	u32 range_start;	
	u32 range_end;		
	u32 processor_capacity;
	u64 memory_capacity;	
};





struct acpi_table_msdm {
	struct acpi_table_header header;	
};



struct acpi_table_mtmr {
	struct acpi_table_header header;	
};



struct acpi_mtmr_entry {
	struct acpi_generic_address physical_address;
	u32 frequency;
	u32 irq;
};



struct acpi_table_nfit {
	struct acpi_table_header header;	
	u32 reserved;		
};



struct acpi_nfit_header {
	u16 type;
	u16 length;
};



enum acpi_nfit_type {
	ACPI_NFIT_TYPE_SYSTEM_ADDRESS = 0,
	ACPI_NFIT_TYPE_MEMORY_MAP = 1,
	ACPI_NFIT_TYPE_INTERLEAVE = 2,
	ACPI_NFIT_TYPE_SMBIOS = 3,
	ACPI_NFIT_TYPE_CONTROL_REGION = 4,
	ACPI_NFIT_TYPE_DATA_REGION = 5,
	ACPI_NFIT_TYPE_FLUSH_ADDRESS = 6,
	ACPI_NFIT_TYPE_CAPABILITIES = 7,
	ACPI_NFIT_TYPE_RESERVED = 8	
};





struct acpi_nfit_system_address {
	struct acpi_nfit_header header;
	u16 range_index;
	u16 flags;
	u32 reserved;		
	u32 proximity_domain;
	u8 range_guid[16];
	u64 address;
	u64 length;
	u64 memory_mapping;
};



#define ACPI_NFIT_ADD_ONLINE_ONLY       (1)	
#define ACPI_NFIT_PROXIMITY_VALID       (1<<1)	





struct acpi_nfit_memory_map {
	struct acpi_nfit_header header;
	u32 device_handle;
	u16 physical_id;
	u16 region_id;
	u16 range_index;
	u16 region_index;
	u64 region_size;
	u64 region_offset;
	u64 address;
	u16 interleave_index;
	u16 interleave_ways;
	u16 flags;
	u16 reserved;		
};



#define ACPI_NFIT_MEM_SAVE_FAILED       (1)	
#define ACPI_NFIT_MEM_RESTORE_FAILED    (1<<1)	
#define ACPI_NFIT_MEM_FLUSH_FAILED      (1<<2)	
#define ACPI_NFIT_MEM_NOT_ARMED         (1<<3)	
#define ACPI_NFIT_MEM_HEALTH_OBSERVED   (1<<4)	
#define ACPI_NFIT_MEM_HEALTH_ENABLED    (1<<5)	
#define ACPI_NFIT_MEM_MAP_FAILED        (1<<6)	



struct acpi_nfit_interleave {
	struct acpi_nfit_header header;
	u16 interleave_index;
	u16 reserved;		
	u32 line_count;
	u32 line_size;
	u32 line_offset[1];	
};



struct acpi_nfit_smbios {
	struct acpi_nfit_header header;
	u32 reserved;		
	u8 data[1];		
};



struct acpi_nfit_control_region {
	struct acpi_nfit_header header;
	u16 region_index;
	u16 vendor_id;
	u16 device_id;
	u16 revision_id;
	u16 subsystem_vendor_id;
	u16 subsystem_device_id;
	u16 subsystem_revision_id;
	u8 valid_fields;
	u8 manufacturing_location;
	u16 manufacturing_date;
	u8 reserved[2];		
	u32 serial_number;
	u16 code;
	u16 windows;
	u64 window_size;
	u64 command_offset;
	u64 command_size;
	u64 status_offset;
	u64 status_size;
	u16 flags;
	u8 reserved1[6];	
};



#define ACPI_NFIT_CONTROL_BUFFERED          (1)	



#define ACPI_NFIT_CONTROL_MFG_INFO_VALID    (1)	



struct acpi_nfit_data_region {
	struct acpi_nfit_header header;
	u16 region_index;
	u16 windows;
	u64 offset;
	u64 size;
	u64 capacity;
	u64 start_address;
};



struct acpi_nfit_flush_address {
	struct acpi_nfit_header header;
	u32 device_handle;
	u16 hint_count;
	u8 reserved[6];		
	u64 hint_address[1];	
};



struct acpi_nfit_capabilities {
	struct acpi_nfit_header header;
	u8 highest_capability;
	u8 reserved[3];		
	u32 capabilities;
	u32 reserved2;
};



#define ACPI_NFIT_CAPABILITY_CACHE_FLUSH       (1)	
#define ACPI_NFIT_CAPABILITY_MEM_FLUSH         (1<<1)	
#define ACPI_NFIT_CAPABILITY_MEM_MIRRORING     (1<<2)	


struct nfit_device_handle {
	u32 handle;
};



#define ACPI_NFIT_DIMM_NUMBER_MASK              0x0000000F
#define ACPI_NFIT_CHANNEL_NUMBER_MASK           0x000000F0
#define ACPI_NFIT_MEMORY_ID_MASK                0x00000F00
#define ACPI_NFIT_SOCKET_ID_MASK                0x0000F000
#define ACPI_NFIT_NODE_ID_MASK                  0x0FFF0000

#define ACPI_NFIT_DIMM_NUMBER_OFFSET            0
#define ACPI_NFIT_CHANNEL_NUMBER_OFFSET         4
#define ACPI_NFIT_MEMORY_ID_OFFSET              8
#define ACPI_NFIT_SOCKET_ID_OFFSET              12
#define ACPI_NFIT_NODE_ID_OFFSET                16



#define ACPI_NFIT_BUILD_DEVICE_HANDLE(dimm, channel, memory, socket, node) \
	((dimm)                                         | \
	((channel) << ACPI_NFIT_CHANNEL_NUMBER_OFFSET)  | \
	((memory)  << ACPI_NFIT_MEMORY_ID_OFFSET)       | \
	((socket)  << ACPI_NFIT_SOCKET_ID_OFFSET)       | \
	((node)    << ACPI_NFIT_NODE_ID_OFFSET))



#define ACPI_NFIT_GET_DIMM_NUMBER(handle) \
	((handle) & ACPI_NFIT_DIMM_NUMBER_MASK)

#define ACPI_NFIT_GET_CHANNEL_NUMBER(handle) \
	(((handle) & ACPI_NFIT_CHANNEL_NUMBER_MASK) >> ACPI_NFIT_CHANNEL_NUMBER_OFFSET)

#define ACPI_NFIT_GET_MEMORY_ID(handle) \
	(((handle) & ACPI_NFIT_MEMORY_ID_MASK)      >> ACPI_NFIT_MEMORY_ID_OFFSET)

#define ACPI_NFIT_GET_SOCKET_ID(handle) \
	(((handle) & ACPI_NFIT_SOCKET_ID_MASK)      >> ACPI_NFIT_SOCKET_ID_OFFSET)

#define ACPI_NFIT_GET_NODE_ID(handle) \
	(((handle) & ACPI_NFIT_NODE_ID_MASK)        >> ACPI_NFIT_NODE_ID_OFFSET)



struct acpi_table_pcct {
	struct acpi_table_header header;	
	u32 flags;
	u64 reserved;
};



#define ACPI_PCCT_DOORBELL              1



enum acpi_pcct_type {
	ACPI_PCCT_TYPE_GENERIC_SUBSPACE = 0,
	ACPI_PCCT_TYPE_HW_REDUCED_SUBSPACE = 1,
	ACPI_PCCT_TYPE_HW_REDUCED_SUBSPACE_TYPE2 = 2,	
	ACPI_PCCT_TYPE_EXT_PCC_MASTER_SUBSPACE = 3,	
	ACPI_PCCT_TYPE_EXT_PCC_SLAVE_SUBSPACE = 4,	
	ACPI_PCCT_TYPE_RESERVED = 5	
};





struct acpi_pcct_subspace {
	struct acpi_subtable_header header;
	u8 reserved[6];
	u64 base_address;
	u64 length;
	struct acpi_generic_address doorbell_register;
	u64 preserve_mask;
	u64 write_mask;
	u32 latency;
	u32 max_access_rate;
	u16 min_turnaround_time;
};



struct acpi_pcct_hw_reduced {
	struct acpi_subtable_header header;
	u32 platform_interrupt;
	u8 flags;
	u8 reserved;
	u64 base_address;
	u64 length;
	struct acpi_generic_address doorbell_register;
	u64 preserve_mask;
	u64 write_mask;
	u32 latency;
	u32 max_access_rate;
	u16 min_turnaround_time;
};



struct acpi_pcct_hw_reduced_type2 {
	struct acpi_subtable_header header;
	u32 platform_interrupt;
	u8 flags;
	u8 reserved;
	u64 base_address;
	u64 length;
	struct acpi_generic_address doorbell_register;
	u64 preserve_mask;
	u64 write_mask;
	u32 latency;
	u32 max_access_rate;
	u16 min_turnaround_time;
	struct acpi_generic_address platform_ack_register;
	u64 ack_preserve_mask;
	u64 ack_write_mask;
};



struct acpi_pcct_ext_pcc_master {
	struct acpi_subtable_header header;
	u32 platform_interrupt;
	u8 flags;
	u8 reserved1;
	u64 base_address;
	u32 length;
	struct acpi_generic_address doorbell_register;
	u64 preserve_mask;
	u64 write_mask;
	u32 latency;
	u32 max_access_rate;
	u32 min_turnaround_time;
	struct acpi_generic_address platform_ack_register;
	u64 ack_preserve_mask;
	u64 ack_set_mask;
	u64 reserved2;
	struct acpi_generic_address cmd_complete_register;
	u64 cmd_complete_mask;
	struct acpi_generic_address cmd_update_register;
	u64 cmd_update_preserve_mask;
	u64 cmd_update_set_mask;
	struct acpi_generic_address error_status_register;
	u64 error_status_mask;
};



struct acpi_pcct_ext_pcc_slave {
	struct acpi_subtable_header header;
	u32 platform_interrupt;
	u8 flags;
	u8 reserved1;
	u64 base_address;
	u32 length;
	struct acpi_generic_address doorbell_register;
	u64 preserve_mask;
	u64 write_mask;
	u32 latency;
	u32 max_access_rate;
	u32 min_turnaround_time;
	struct acpi_generic_address platform_ack_register;
	u64 ack_preserve_mask;
	u64 ack_set_mask;
	u64 reserved2;
	struct acpi_generic_address cmd_complete_register;
	u64 cmd_complete_mask;
	struct acpi_generic_address cmd_update_register;
	u64 cmd_update_preserve_mask;
	u64 cmd_update_set_mask;
	struct acpi_generic_address error_status_register;
	u64 error_status_mask;
};



#define ACPI_PCCT_INTERRUPT_POLARITY    (1)
#define ACPI_PCCT_INTERRUPT_MODE        (1<<1)





struct acpi_pcct_shared_memory {
	u32 signature;
	u16 command;
	u16 status;
};



struct acpi_pcct_ext_pcc_shared_memory {
	u32 signature;
	u32 flags;
	u32 length;
	u32 command;
};



struct acpi_table_pdtt {
	struct acpi_table_header header;	
	u8 trigger_count;
	u8 reserved[3];
	u32 array_offset;
};


struct acpi_pdtt_channel {
	u8 subchannel_id;
	u8 flags;
};



#define ACPI_PDTT_RUNTIME_TRIGGER           (1)
#define ACPI_PDTT_WAIT_COMPLETION           (1<<1)
#define ACPI_PDTT_TRIGGER_ORDER             (1<<2)



struct acpi_table_pmtt {
	struct acpi_table_header header;	
	u32 reserved;
};



struct acpi_pmtt_header {
	u8 type;
	u8 reserved1;
	u16 length;
	u16 flags;
	u16 reserved2;
};



#define ACPI_PMTT_TYPE_SOCKET           0
#define ACPI_PMTT_TYPE_CONTROLLER       1
#define ACPI_PMTT_TYPE_DIMM             2
#define ACPI_PMTT_TYPE_RESERVED         3	



#define ACPI_PMTT_TOP_LEVEL             0x0001
#define ACPI_PMTT_PHYSICAL              0x0002
#define ACPI_PMTT_MEMORY_TYPE           0x000C





struct acpi_pmtt_socket {
	struct acpi_pmtt_header header;
	u16 socket_id;
	u16 reserved;
};



struct acpi_pmtt_controller {
	struct acpi_pmtt_header header;
	u32 read_latency;
	u32 write_latency;
	u32 read_bandwidth;
	u32 write_bandwidth;
	u16 access_width;
	u16 alignment;
	u16 reserved;
	u16 domain_count;
};



struct acpi_pmtt_domain {
	u32 proximity_domain;
};



struct acpi_pmtt_physical_component {
	struct acpi_pmtt_header header;
	u16 component_id;
	u16 reserved;
	u32 memory_size;
	u32 bios_handle;
};



struct acpi_table_pptt {
	struct acpi_table_header header;	
};



enum acpi_pptt_type {
	ACPI_PPTT_TYPE_PROCESSOR = 0,
	ACPI_PPTT_TYPE_CACHE = 1,
	ACPI_PPTT_TYPE_ID = 2,
	ACPI_PPTT_TYPE_RESERVED = 3
};



struct acpi_pptt_processor {
	struct acpi_subtable_header header;
	u16 reserved;
	u32 flags;
	u32 parent;
	u32 acpi_processor_id;
	u32 number_of_priv_resources;
};



#define ACPI_PPTT_PHYSICAL_PACKAGE          (1)
#define ACPI_PPTT_ACPI_PROCESSOR_ID_VALID   (1<<1)
#define ACPI_PPTT_ACPI_PROCESSOR_IS_THREAD  (1<<2)	
#define ACPI_PPTT_ACPI_LEAF_NODE            (1<<3)	
#define ACPI_PPTT_ACPI_IDENTICAL            (1<<4)	



struct acpi_pptt_cache {
	struct acpi_subtable_header header;
	u16 reserved;
	u32 flags;
	u32 next_level_of_cache;
	u32 size;
	u32 number_of_sets;
	u8 associativity;
	u8 attributes;
	u16 line_size;
};



#define ACPI_PPTT_SIZE_PROPERTY_VALID       (1)	
#define ACPI_PPTT_NUMBER_OF_SETS_VALID      (1<<1)	
#define ACPI_PPTT_ASSOCIATIVITY_VALID       (1<<2)	
#define ACPI_PPTT_ALLOCATION_TYPE_VALID     (1<<3)	
#define ACPI_PPTT_CACHE_TYPE_VALID          (1<<4)	
#define ACPI_PPTT_WRITE_POLICY_VALID        (1<<5)	
#define ACPI_PPTT_LINE_SIZE_VALID           (1<<6)	



#define ACPI_PPTT_MASK_ALLOCATION_TYPE      (0x03)	
#define ACPI_PPTT_MASK_CACHE_TYPE           (0x0C)	
#define ACPI_PPTT_MASK_WRITE_POLICY         (0x10)	


#define ACPI_PPTT_CACHE_READ_ALLOCATE       (0x0)	
#define ACPI_PPTT_CACHE_WRITE_ALLOCATE      (0x01)	
#define ACPI_PPTT_CACHE_RW_ALLOCATE         (0x02)	
#define ACPI_PPTT_CACHE_RW_ALLOCATE_ALT     (0x03)	

#define ACPI_PPTT_CACHE_TYPE_DATA           (0x0)	
#define ACPI_PPTT_CACHE_TYPE_INSTR          (1<<2)	
#define ACPI_PPTT_CACHE_TYPE_UNIFIED        (2<<2)	
#define ACPI_PPTT_CACHE_TYPE_UNIFIED_ALT    (3<<2)	

#define ACPI_PPTT_CACHE_POLICY_WB           (0x0)	
#define ACPI_PPTT_CACHE_POLICY_WT           (1<<4)	



struct acpi_pptt_id {
	struct acpi_subtable_header header;
	u16 reserved;
	u32 vendor_id;
	u64 level1_id;
	u64 level2_id;
	u16 major_rev;
	u16 minor_rev;
	u16 spin_rev;
};



struct acpi_table_rasf {
	struct acpi_table_header header;	
	u8 channel_id[12];
};



struct acpi_rasf_shared_memory {
	u32 signature;
	u16 command;
	u16 status;
	u16 version;
	u8 capabilities[16];
	u8 set_capabilities[16];
	u16 num_parameter_blocks;
	u32 set_capabilities_status;
};



struct acpi_rasf_parameter_block {
	u16 type;
	u16 version;
	u16 length;
};



struct acpi_rasf_patrol_scrub_parameter {
	struct acpi_rasf_parameter_block header;
	u16 patrol_scrub_command;
	u64 requested_address_range[2];
	u64 actual_address_range[2];
	u16 flags;
	u8 requested_speed;
};



#define ACPI_RASF_SCRUBBER_RUNNING      1
#define ACPI_RASF_SPEED                 (7<<1)
#define ACPI_RASF_SPEED_SLOW            (0<<1)
#define ACPI_RASF_SPEED_MEDIUM          (4<<1)
#define ACPI_RASF_SPEED_FAST            (7<<1)



enum acpi_rasf_commands {
	ACPI_RASF_EXECUTE_RASF_COMMAND = 1
};



enum acpi_rasf_capabiliities {
	ACPI_HW_PATROL_SCRUB_SUPPORTED = 0,
	ACPI_SW_PATROL_SCRUB_EXPOSED = 1
};



enum acpi_rasf_patrol_scrub_commands {
	ACPI_RASF_GET_PATROL_PARAMETERS = 1,
	ACPI_RASF_START_PATROL_SCRUBBER = 2,
	ACPI_RASF_STOP_PATROL_SCRUBBER = 3
};



#define ACPI_RASF_GENERATE_SCI          (1<<15)



enum acpi_rasf_status {
	ACPI_RASF_SUCCESS = 0,
	ACPI_RASF_NOT_VALID = 1,
	ACPI_RASF_NOT_SUPPORTED = 2,
	ACPI_RASF_BUSY = 3,
	ACPI_RASF_FAILED = 4,
	ACPI_RASF_ABORTED = 5,
	ACPI_RASF_INVALID_DATA = 6
};



#define ACPI_RASF_COMMAND_COMPLETE      (1)
#define ACPI_RASF_SCI_DOORBELL          (1<<1)
#define ACPI_RASF_ERROR                 (1<<2)
#define ACPI_RASF_STATUS                (0x1F<<3)



struct acpi_table_sbst {
	struct acpi_table_header header;	
	u32 warning_level;
	u32 low_level;
	u32 critical_level;
};



struct acpi_table_sdei {
	struct acpi_table_header header;	
};



struct acpi_table_sdev {
	struct acpi_table_header header;	
};

struct acpi_sdev_header {
	u8 type;
	u8 flags;
	u16 length;
};



enum acpi_sdev_type {
	ACPI_SDEV_TYPE_NAMESPACE_DEVICE = 0,
	ACPI_SDEV_TYPE_PCIE_ENDPOINT_DEVICE = 1,
	ACPI_SDEV_TYPE_RESERVED = 2	
};



#define ACPI_SDEV_HANDOFF_TO_UNSECURE_OS    (1)





struct acpi_sdev_namespace {
	struct acpi_sdev_header header;
	u16 device_id_offset;
	u16 device_id_length;
	u16 vendor_data_offset;
	u16 vendor_data_length;
};



struct acpi_sdev_pcie {
	struct acpi_sdev_header header;
	u16 segment;
	u16 start_bus;
	u16 path_offset;
	u16 path_length;
	u16 vendor_data_offset;
	u16 vendor_data_length;
};



struct acpi_sdev_pcie_path {
	u8 device;
	u8 function;
};



#pragma pack()

#endif				
