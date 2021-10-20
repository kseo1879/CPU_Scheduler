/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __LINUX_SPI_ALTERA_H
#define __LINUX_SPI_ALTERA_H

#include <linux/regmap.h>
#include <linux/spi/spi.h>
#include <linux/types.h>


struct altera_spi_platform_data {
	u16				mode_bits;
	u16				num_chipselect;
	u32				bits_per_word_mask;
	u16				num_devices;
	struct spi_board_info		*devices;
};

#endif 
