/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _MPEG2_CTRLS_H_
#define _MPEG2_CTRLS_H_

#define V4L2_CID_MPEG_VIDEO_MPEG2_SLICE_PARAMS		(V4L2_CID_MPEG_BASE+250)
#define V4L2_CID_MPEG_VIDEO_MPEG2_QUANTIZATION		(V4L2_CID_MPEG_BASE+251)


#define V4L2_CTRL_TYPE_MPEG2_SLICE_PARAMS 0x0103
#define	V4L2_CTRL_TYPE_MPEG2_QUANTIZATION 0x0104

#define V4L2_MPEG2_PICTURE_CODING_TYPE_I	1
#define V4L2_MPEG2_PICTURE_CODING_TYPE_P	2
#define V4L2_MPEG2_PICTURE_CODING_TYPE_B	3
#define V4L2_MPEG2_PICTURE_CODING_TYPE_D	4

struct v4l2_mpeg2_sequence {
	
	__u16	horizontal_size;
	__u16	vertical_size;
	__u32	vbv_buffer_size;

	
	__u16	profile_and_level_indication;
	__u8	progressive_sequence;
	__u8	chroma_format;
};

struct v4l2_mpeg2_picture {
	
	__u8	picture_coding_type;

	
	__u8	f_code[2][2];
	__u8	intra_dc_precision;
	__u8	picture_structure;
	__u8	top_field_first;
	__u8	frame_pred_frame_dct;
	__u8	concealment_motion_vectors;
	__u8	q_scale_type;
	__u8	intra_vlc_format;
	__u8	alternate_scan;
	__u8	repeat_first_field;
	__u16	progressive_frame;
};

struct v4l2_ctrl_mpeg2_slice_params {
	__u32	bit_size;
	__u32	data_bit_offset;
	__u64	backward_ref_ts;
	__u64	forward_ref_ts;

	struct v4l2_mpeg2_sequence sequence;
	struct v4l2_mpeg2_picture picture;

	
	__u32	quantiser_scale_code;
};

struct v4l2_ctrl_mpeg2_quantization {
	
	__u8	load_intra_quantiser_matrix;
	__u8	load_non_intra_quantiser_matrix;
	__u8	load_chroma_intra_quantiser_matrix;
	__u8	load_chroma_non_intra_quantiser_matrix;

	__u8	intra_quantiser_matrix[64];
	__u8	non_intra_quantiser_matrix[64];
	__u8	chroma_intra_quantiser_matrix[64];
	__u8	chroma_non_intra_quantiser_matrix[64];
};

#endif
