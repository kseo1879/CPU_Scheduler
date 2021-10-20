/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef __IIO_DMAENGINE_H__
#define __IIO_DMAENGINE_H__

struct iio_buffer;
struct device;

struct iio_buffer *devm_iio_dmaengine_buffer_alloc(struct device *dev,
						   const char *channel);

#endif
