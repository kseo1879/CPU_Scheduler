/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_INSN_H
#define _ASM_X86_INSN_H



#include <asm/inat.h>

struct insn_field {
	union {
		insn_value_t value;
		insn_byte_t bytes[4];
	};
	
	unsigned char got;
	unsigned char nbytes;
};

struct insn {
	struct insn_field prefixes;	
	struct insn_field rex_prefix;	
	struct insn_field vex_prefix;	
	struct insn_field opcode;	
	struct insn_field modrm;
	struct insn_field sib;
	struct insn_field displacement;
	union {
		struct insn_field immediate;
		struct insn_field moffset1;	
		struct insn_field immediate1;	
	};
	union {
		struct insn_field moffset2;	
		struct insn_field immediate2;	
	};

	int	emulate_prefix_size;
	insn_attr_t attr;
	unsigned char opnd_bytes;
	unsigned char addr_bytes;
	unsigned char length;
	unsigned char x86_64;

	const insn_byte_t *kaddr;	
	const insn_byte_t *end_kaddr;	
	const insn_byte_t *next_byte;
};

#define MAX_INSN_SIZE	15

#define X86_MODRM_MOD(modrm) (((modrm) & 0xc0) >> 6)
#define X86_MODRM_REG(modrm) (((modrm) & 0x38) >> 3)
#define X86_MODRM_RM(modrm) ((modrm) & 0x07)

#define X86_SIB_SCALE(sib) (((sib) & 0xc0) >> 6)
#define X86_SIB_INDEX(sib) (((sib) & 0x38) >> 3)
#define X86_SIB_BASE(sib) ((sib) & 0x07)

#define X86_REX_W(rex) ((rex) & 8)
#define X86_REX_R(rex) ((rex) & 4)
#define X86_REX_X(rex) ((rex) & 2)
#define X86_REX_B(rex) ((rex) & 1)


#define X86_VEX_W(vex)	((vex) & 0x80)	
#define X86_VEX_R(vex)	((vex) & 0x80)	
#define X86_VEX_X(vex)	((vex) & 0x40)	
#define X86_VEX_B(vex)	((vex) & 0x20)	
#define X86_VEX_L(vex)	((vex) & 0x04)	

#define X86_EVEX_M(vex)	((vex) & 0x03)		
#define X86_VEX3_M(vex)	((vex) & 0x1f)		
#define X86_VEX2_M	1			
#define X86_VEX_V(vex)	(((vex) & 0x78) >> 3)	
#define X86_VEX_P(vex)	((vex) & 0x03)		
#define X86_VEX_M_MAX	0x1f			

extern void insn_init(struct insn *insn, const void *kaddr, int buf_len, int x86_64);
extern void insn_get_prefixes(struct insn *insn);
extern void insn_get_opcode(struct insn *insn);
extern void insn_get_modrm(struct insn *insn);
extern void insn_get_sib(struct insn *insn);
extern void insn_get_displacement(struct insn *insn);
extern void insn_get_immediate(struct insn *insn);
extern void insn_get_length(struct insn *insn);


static inline void insn_get_attribute(struct insn *insn)
{
	insn_get_modrm(insn);
}


extern int insn_rip_relative(struct insn *insn);


static inline void kernel_insn_init(struct insn *insn,
				    const void *kaddr, int buf_len)
{
#ifdef CONFIG_X86_64
	insn_init(insn, kaddr, buf_len, 1);
#else 
	insn_init(insn, kaddr, buf_len, 0);
#endif
}

static inline int insn_is_avx(struct insn *insn)
{
	if (!insn->prefixes.got)
		insn_get_prefixes(insn);
	return (insn->vex_prefix.value != 0);
}

static inline int insn_is_evex(struct insn *insn)
{
	if (!insn->prefixes.got)
		insn_get_prefixes(insn);
	return (insn->vex_prefix.nbytes == 4);
}

static inline int insn_has_emulate_prefix(struct insn *insn)
{
	return !!insn->emulate_prefix_size;
}


static inline int insn_complete(struct insn *insn)
{
	return insn->opcode.got && insn->modrm.got && insn->sib.got &&
		insn->displacement.got && insn->immediate.got;
}

static inline insn_byte_t insn_vex_m_bits(struct insn *insn)
{
	if (insn->vex_prefix.nbytes == 2)	
		return X86_VEX2_M;
	else if (insn->vex_prefix.nbytes == 3)	
		return X86_VEX3_M(insn->vex_prefix.bytes[1]);
	else					
		return X86_EVEX_M(insn->vex_prefix.bytes[1]);
}

static inline insn_byte_t insn_vex_p_bits(struct insn *insn)
{
	if (insn->vex_prefix.nbytes == 2)	
		return X86_VEX_P(insn->vex_prefix.bytes[1]);
	else
		return X86_VEX_P(insn->vex_prefix.bytes[2]);
}


static inline int insn_last_prefix_id(struct insn *insn)
{
	if (insn_is_avx(insn))
		return insn_vex_p_bits(insn);	

	if (insn->prefixes.bytes[3])
		return inat_get_last_prefix_id(insn->prefixes.bytes[3]);

	return 0;
}


static inline int insn_offset_rex_prefix(struct insn *insn)
{
	return insn->prefixes.nbytes;
}
static inline int insn_offset_vex_prefix(struct insn *insn)
{
	return insn_offset_rex_prefix(insn) + insn->rex_prefix.nbytes;
}
static inline int insn_offset_opcode(struct insn *insn)
{
	return insn_offset_vex_prefix(insn) + insn->vex_prefix.nbytes;
}
static inline int insn_offset_modrm(struct insn *insn)
{
	return insn_offset_opcode(insn) + insn->opcode.nbytes;
}
static inline int insn_offset_sib(struct insn *insn)
{
	return insn_offset_modrm(insn) + insn->modrm.nbytes;
}
static inline int insn_offset_displacement(struct insn *insn)
{
	return insn_offset_sib(insn) + insn->sib.nbytes;
}
static inline int insn_offset_immediate(struct insn *insn)
{
	return insn_offset_displacement(insn) + insn->displacement.nbytes;
}


#define for_each_insn_prefix(insn, idx, prefix)	\
	for (idx = 0; idx < ARRAY_SIZE(insn->prefixes.bytes) && (prefix = insn->prefixes.bytes[idx]) != 0; idx++)

#define POP_SS_OPCODE 0x1f
#define MOV_SREG_OPCODE 0x8e


static inline int insn_masking_exception(struct insn *insn)
{
	return insn->opcode.bytes[0] == POP_SS_OPCODE ||
		(insn->opcode.bytes[0] == MOV_SREG_OPCODE &&
		 X86_MODRM_REG(insn->modrm.bytes[0]) == 2);
}

#endif 
