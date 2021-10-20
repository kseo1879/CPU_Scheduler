/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _LINUX_SUNRPC_GSS_KRB5_ENCTYPES_H
#define _LINUX_SUNRPC_GSS_KRB5_ENCTYPES_H

#ifdef CONFIG_SUNRPC_DISABLE_INSECURE_ENCTYPES


#define KRB5_SUPPORTED_ENCTYPES "18,17,16"

#else	


#define KRB5_SUPPORTED_ENCTYPES "18,17,16,3,1,2"

#endif	

#endif	
