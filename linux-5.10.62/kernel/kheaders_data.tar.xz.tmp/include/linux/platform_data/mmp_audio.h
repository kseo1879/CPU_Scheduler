/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef MMP_AUDIO_H
#define MMP_AUDIO_H

struct mmp_audio_platdata {
	u32 period_max_capture;
	u32 buffer_max_capture;
	u32 period_max_playback;
	u32 buffer_max_playback;
};

#endif 
