/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef V4L2_MEDIABUS_H
#define V4L2_MEDIABUS_H

#include <linux/v4l2-mediabus.h>
#include <linux/bitops.h>





#define V4L2_MBUS_MASTER			BIT(0)
#define V4L2_MBUS_SLAVE				BIT(1)

#define V4L2_MBUS_HSYNC_ACTIVE_HIGH		BIT(2)
#define V4L2_MBUS_HSYNC_ACTIVE_LOW		BIT(3)
#define V4L2_MBUS_VSYNC_ACTIVE_HIGH		BIT(4)
#define V4L2_MBUS_VSYNC_ACTIVE_LOW		BIT(5)
#define V4L2_MBUS_PCLK_SAMPLE_RISING		BIT(6)
#define V4L2_MBUS_PCLK_SAMPLE_FALLING		BIT(7)
#define V4L2_MBUS_DATA_ACTIVE_HIGH		BIT(8)
#define V4L2_MBUS_DATA_ACTIVE_LOW		BIT(9)

#define V4L2_MBUS_FIELD_EVEN_HIGH		BIT(10)

#define V4L2_MBUS_FIELD_EVEN_LOW		BIT(11)

#define V4L2_MBUS_VIDEO_SOG_ACTIVE_HIGH		BIT(12)
#define V4L2_MBUS_VIDEO_SOG_ACTIVE_LOW		BIT(13)
#define V4L2_MBUS_DATA_ENABLE_HIGH		BIT(14)
#define V4L2_MBUS_DATA_ENABLE_LOW		BIT(15)



#define V4L2_MBUS_CSI2_1_LANE			BIT(0)
#define V4L2_MBUS_CSI2_2_LANE			BIT(1)
#define V4L2_MBUS_CSI2_3_LANE			BIT(2)
#define V4L2_MBUS_CSI2_4_LANE			BIT(3)

#define V4L2_MBUS_CSI2_CHANNEL_0		BIT(4)
#define V4L2_MBUS_CSI2_CHANNEL_1		BIT(5)
#define V4L2_MBUS_CSI2_CHANNEL_2		BIT(6)
#define V4L2_MBUS_CSI2_CHANNEL_3		BIT(7)

#define V4L2_MBUS_CSI2_CONTINUOUS_CLOCK		BIT(8)
#define V4L2_MBUS_CSI2_NONCONTINUOUS_CLOCK	BIT(9)

#define V4L2_MBUS_CSI2_LANES		(V4L2_MBUS_CSI2_1_LANE | \
					 V4L2_MBUS_CSI2_2_LANE | \
					 V4L2_MBUS_CSI2_3_LANE | \
					 V4L2_MBUS_CSI2_4_LANE)
#define V4L2_MBUS_CSI2_CHANNELS		(V4L2_MBUS_CSI2_CHANNEL_0 | \
					 V4L2_MBUS_CSI2_CHANNEL_1 | \
					 V4L2_MBUS_CSI2_CHANNEL_2 | \
					 V4L2_MBUS_CSI2_CHANNEL_3)


enum v4l2_mbus_type {
	V4L2_MBUS_UNKNOWN,
	V4L2_MBUS_PARALLEL,
	V4L2_MBUS_BT656,
	V4L2_MBUS_CSI1,
	V4L2_MBUS_CCP2,
	V4L2_MBUS_CSI2_DPHY,
	V4L2_MBUS_CSI2_CPHY,
	V4L2_MBUS_INVALID,
};


struct v4l2_mbus_config {
	enum v4l2_mbus_type type;
	unsigned int flags;
};


static inline void
v4l2_fill_pix_format(struct v4l2_pix_format *pix_fmt,
		     const struct v4l2_mbus_framefmt *mbus_fmt)
{
	pix_fmt->width = mbus_fmt->width;
	pix_fmt->height = mbus_fmt->height;
	pix_fmt->field = mbus_fmt->field;
	pix_fmt->colorspace = mbus_fmt->colorspace;
	pix_fmt->ycbcr_enc = mbus_fmt->ycbcr_enc;
	pix_fmt->quantization = mbus_fmt->quantization;
	pix_fmt->xfer_func = mbus_fmt->xfer_func;
}


static inline void v4l2_fill_mbus_format(struct v4l2_mbus_framefmt *mbus_fmt,
					 const struct v4l2_pix_format *pix_fmt,
			   u32 code)
{
	mbus_fmt->width = pix_fmt->width;
	mbus_fmt->height = pix_fmt->height;
	mbus_fmt->field = pix_fmt->field;
	mbus_fmt->colorspace = pix_fmt->colorspace;
	mbus_fmt->ycbcr_enc = pix_fmt->ycbcr_enc;
	mbus_fmt->quantization = pix_fmt->quantization;
	mbus_fmt->xfer_func = pix_fmt->xfer_func;
	mbus_fmt->code = code;
}


static inline void
v4l2_fill_pix_format_mplane(struct v4l2_pix_format_mplane *pix_mp_fmt,
			    const struct v4l2_mbus_framefmt *mbus_fmt)
{
	pix_mp_fmt->width = mbus_fmt->width;
	pix_mp_fmt->height = mbus_fmt->height;
	pix_mp_fmt->field = mbus_fmt->field;
	pix_mp_fmt->colorspace = mbus_fmt->colorspace;
	pix_mp_fmt->ycbcr_enc = mbus_fmt->ycbcr_enc;
	pix_mp_fmt->quantization = mbus_fmt->quantization;
	pix_mp_fmt->xfer_func = mbus_fmt->xfer_func;
}


static inline void
v4l2_fill_mbus_format_mplane(struct v4l2_mbus_framefmt *mbus_fmt,
			     const struct v4l2_pix_format_mplane *pix_mp_fmt)
{
	mbus_fmt->width = pix_mp_fmt->width;
	mbus_fmt->height = pix_mp_fmt->height;
	mbus_fmt->field = pix_mp_fmt->field;
	mbus_fmt->colorspace = pix_mp_fmt->colorspace;
	mbus_fmt->ycbcr_enc = pix_mp_fmt->ycbcr_enc;
	mbus_fmt->quantization = pix_mp_fmt->quantization;
	mbus_fmt->xfer_func = pix_mp_fmt->xfer_func;
}

#endif
