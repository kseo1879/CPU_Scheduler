/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _LINUX_OID_REGISTRY_H
#define _LINUX_OID_REGISTRY_H

#include <linux/types.h>


enum OID {
	OID_id_dsa_with_sha1,		
	OID_id_dsa,			
	OID_id_ecdsa_with_sha1,		
	OID_id_ecPublicKey,		

	
	OID_rsaEncryption,		
	OID_md2WithRSAEncryption,	
	OID_md3WithRSAEncryption,	
	OID_md4WithRSAEncryption,	
	OID_sha1WithRSAEncryption,	
	OID_sha256WithRSAEncryption,	
	OID_sha384WithRSAEncryption,	
	OID_sha512WithRSAEncryption,	
	OID_sha224WithRSAEncryption,	
	
	OID_data,			
	OID_signed_data,		
	
	OID_email_address,		
	OID_contentType,		
	OID_messageDigest,		
	OID_signingTime,		
	OID_smimeCapabilites,		
	OID_smimeAuthenticatedAttrs,	

	
	OID_md2,			
	OID_md4,			
	OID_md5,			

	
	OID_msIndirectData,		
	OID_msStatementType,		
	OID_msSpOpusInfo,		
	OID_msPeImageDataObjId,		
	OID_msIndividualSPKeyPurpose,	
	OID_msOutlookExpress,		

	OID_certAuthInfoAccess,		
	OID_sha1,			
	OID_sha256,			
	OID_sha384,			
	OID_sha512,			
	OID_sha224,			

	
	OID_commonName,			
	OID_surname,			
	OID_countryName,		
	OID_locality,			
	OID_stateOrProvinceName,	
	OID_organizationName,		
	OID_organizationUnitName,	
	OID_title,			
	OID_description,		
	OID_name,			
	OID_givenName,			
	OID_initials,			
	OID_generationalQualifier,	

	
	OID_subjectKeyIdentifier,	
	OID_keyUsage,			
	OID_subjectAltName,		
	OID_issuerAltName,		
	OID_basicConstraints,		
	OID_crlDistributionPoints,	
	OID_certPolicies,		
	OID_authorityKeyIdentifier,	
	OID_extKeyUsage,		

	
	OID_gostCPSignA,		
	OID_gostCPSignB,		
	OID_gostCPSignC,		
	OID_gost2012PKey256,		
	OID_gost2012PKey512,		
	OID_gost2012Digest256,		
	OID_gost2012Digest512,		
	OID_gost2012Signature256,	
	OID_gost2012Signature512,	
	OID_gostTC26Sign256A,		
	OID_gostTC26Sign256B,		
	OID_gostTC26Sign256C,		
	OID_gostTC26Sign256D,		
	OID_gostTC26Sign512A,		
	OID_gostTC26Sign512B,		
	OID_gostTC26Sign512C,		

	
	OID_sm2,			
	OID_sm3,			
	OID_SM2_with_SM3,		
	OID_sm3WithRSAEncryption,	

	OID__NR
};

extern enum OID look_up_OID(const void *data, size_t datasize);
extern int sprint_oid(const void *, size_t, char *, size_t);
extern int sprint_OID(enum OID, char *, size_t);

#endif 
