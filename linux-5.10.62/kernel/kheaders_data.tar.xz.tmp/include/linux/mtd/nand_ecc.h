/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __MTD_NAND_ECC_H__
#define __MTD_NAND_ECC_H__

struct nand_chip;


void __nand_calculate_ecc(const u_char *dat, unsigned int eccsize,
			  u_char *ecc_code, bool sm_order);


int nand_calculate_ecc(struct nand_chip *chip, const u_char *dat,
		       u_char *ecc_code);


int __nand_correct_data(u_char *dat, u_char *read_ecc, u_char *calc_ecc,
			unsigned int eccsize, bool sm_order);


int nand_correct_data(struct nand_chip *chip, u_char *dat, u_char *read_ecc,
		      u_char *calc_ecc);

#endif 
