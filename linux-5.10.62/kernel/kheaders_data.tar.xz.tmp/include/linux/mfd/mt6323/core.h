/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __MFD_MT6323_CORE_H__
#define __MFD_MT6323_CORE_H__

enum MT6323_IRQ_STATUS_numbers {
	MT6323_IRQ_STATUS_SPKL_AB = 0,
	MT6323_IRQ_STATUS_SPKL,
	MT6323_IRQ_STATUS_BAT_L,
	MT6323_IRQ_STATUS_BAT_H,
	MT6323_IRQ_STATUS_WATCHDOG,
	MT6323_IRQ_STATUS_PWRKEY,
	MT6323_IRQ_STATUS_THR_L,
	MT6323_IRQ_STATUS_THR_H,
	MT6323_IRQ_STATUS_VBATON_UNDET,
	MT6323_IRQ_STATUS_BVALID_DET,
	MT6323_IRQ_STATUS_CHRDET,
	MT6323_IRQ_STATUS_OV,
	MT6323_IRQ_STATUS_LDO = 16,
	MT6323_IRQ_STATUS_FCHRKEY,
	MT6323_IRQ_STATUS_ACCDET,
	MT6323_IRQ_STATUS_AUDIO,
	MT6323_IRQ_STATUS_RTC,
	MT6323_IRQ_STATUS_VPROC,
	MT6323_IRQ_STATUS_VSYS,
	MT6323_IRQ_STATUS_VPA,
	MT6323_IRQ_STATUS_NR,
};

#endif 
