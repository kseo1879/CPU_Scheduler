#ifndef __ASM_OFFSETS_H__
#define __ASM_OFFSETS_H__



#define pt_regs_bx 40 
#define pt_regs_cx 88 
#define pt_regs_dx 96 
#define pt_regs_sp 152 
#define pt_regs_bp 32 
#define pt_regs_si 104 
#define pt_regs_di 112 
#define pt_regs_r8 72 
#define pt_regs_r9 64 
#define pt_regs_r10 56 
#define pt_regs_r11 48 
#define pt_regs_r12 24 
#define pt_regs_r13 16 
#define pt_regs_r14 8 
#define pt_regs_r15 0 
#define pt_regs_flags 144 

#define saved_context_cr0 200 
#define saved_context_cr2 208 
#define saved_context_cr3 216 
#define saved_context_cr4 224 
#define saved_context_gdt_desc 267 



#define TASK_threadsp 2072 

#define crypto_tfm_ctx_offset 24 

#define pbe_address 0 
#define pbe_orig_address 8 
#define pbe_next 16 

#define BP_scratch 484 
#define BP_secure_boot 492 
#define BP_loadflags 529 
#define BP_hardware_subarch 572 
#define BP_version 518 
#define BP_kernel_alignment 560 
#define BP_init_size 608 
#define BP_pref_address 600 

#define PTREGS_SIZE 168 
#define TLB_STATE_user_pcid_flush_mask 22 
#define CPU_ENTRY_AREA_entry_stack 4096 
#define SIZEOF_entry_stack 4096 
#define MASK_entry_stack -4096 
#define TSS_sp0 4 
#define TSS_sp1 12 
#define TSS_sp2 20 

#endif
