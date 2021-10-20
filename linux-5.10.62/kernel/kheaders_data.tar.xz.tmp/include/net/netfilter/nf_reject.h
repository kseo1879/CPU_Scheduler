/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _NF_REJECT_H
#define _NF_REJECT_H

#include <linux/types.h>
#include <uapi/linux/in.h>

static inline bool nf_reject_verify_csum(__u8 proto)
{
	
	switch (proto) {
		
		case IPPROTO_AH:
		case IPPROTO_ESP:
		case IPPROTO_SCTP:

		
		case IPPROTO_UDPLITE:
		case IPPROTO_DCCP:

		
		case IPPROTO_GRE:
			return false;
	}
	return true;
}

#endif 
