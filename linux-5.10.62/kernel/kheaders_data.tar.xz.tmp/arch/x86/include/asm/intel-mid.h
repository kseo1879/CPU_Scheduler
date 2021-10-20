/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _ASM_X86_INTEL_MID_H
#define _ASM_X86_INTEL_MID_H

#include <linux/sfi.h>
#include <linux/pci.h>
#include <linux/platform_device.h>

extern int intel_mid_pci_init(void);
extern int intel_mid_pci_set_power_state(struct pci_dev *pdev, pci_power_t state);
extern pci_power_t intel_mid_pci_get_power_state(struct pci_dev *pdev);

extern void intel_mid_pwr_power_off(void);

#define INTEL_MID_PWR_LSS_OFFSET	4
#define INTEL_MID_PWR_LSS_TYPE		(1 << 7)

extern int intel_mid_pwr_get_lss_id(struct pci_dev *pdev);

extern int get_gpio_by_name(const char *name);
extern int __init sfi_parse_mrtc(struct sfi_table_header *table);
extern int __init sfi_parse_mtmr(struct sfi_table_header *table);
extern int sfi_mrtc_num;
extern struct sfi_rtc_table_entry sfi_mrtc_array[];


struct devs_id {
	char name[SFI_NAME_LEN + 1];
	u8 type;
	u8 delay;
	u8 msic;
	void *(*get_platform_data)(void *info);
};

#define sfi_device(i)								\
	static const struct devs_id *const __intel_mid_sfi_##i##_dev __used	\
	__section(".x86_intel_mid_dev.init") = &i


struct mid_sd_board_info {
	char		name[SFI_NAME_LEN];
	int		bus_num;
	unsigned short	addr;
	u32		max_clk;
	void		*platform_data;
};


enum intel_mid_cpu_type {
	
	INTEL_MID_CPU_CHIP_PENWELL = 2,
	INTEL_MID_CPU_CHIP_CLOVERVIEW,
	INTEL_MID_CPU_CHIP_TANGIER,
};

extern enum intel_mid_cpu_type __intel_mid_cpu_chip;

#ifdef CONFIG_X86_INTEL_MID

static inline enum intel_mid_cpu_type intel_mid_identify_cpu(void)
{
	return __intel_mid_cpu_chip;
}

static inline bool intel_mid_has_msic(void)
{
	return (intel_mid_identify_cpu() == INTEL_MID_CPU_CHIP_PENWELL);
}

extern void intel_scu_devices_create(void);
extern void intel_scu_devices_destroy(void);

#else 

#define intel_mid_identify_cpu()	0
#define intel_mid_has_msic()		0

static inline void intel_scu_devices_create(void) { }
static inline void intel_scu_devices_destroy(void) { }

#endif 

enum intel_mid_timer_options {
	INTEL_MID_TIMER_DEFAULT,
	INTEL_MID_TIMER_APBT_ONLY,
	INTEL_MID_TIMER_LAPIC_APBT,
};

extern enum intel_mid_timer_options intel_mid_timer_options;


#define BSEL_SOC_FUSE_MASK		0x7

#define BSEL_SOC_FUSE_001		0x1

#define BSEL_SOC_FUSE_101		0x5

#define BSEL_SOC_FUSE_111		0x7

#define SFI_MTMR_MAX_NUM		8
#define SFI_MRTC_MAX			8


#define MRST_VRTC_MAP_SZ		1024


extern void intel_mid_rtc_init(void);


#define INTEL_MID_IRQ_OFFSET		0x100

#endif 
