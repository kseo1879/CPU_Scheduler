/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _CRYPTO_GLUE_HELPER_H
#define _CRYPTO_GLUE_HELPER_H

#include <crypto/internal/skcipher.h>
#include <linux/kernel.h>
#include <asm/fpu/api.h>
#include <crypto/b128ops.h>

typedef void (*common_glue_func_t)(const void *ctx, u8 *dst, const u8 *src);
typedef void (*common_glue_cbc_func_t)(const void *ctx, u8 *dst, const u8 *src);
typedef void (*common_glue_ctr_func_t)(const void *ctx, u8 *dst, const u8 *src,
				       le128 *iv);
typedef void (*common_glue_xts_func_t)(const void *ctx, u8 *dst, const u8 *src,
				       le128 *iv);

struct common_glue_func_entry {
	unsigned int num_blocks; 
	union {
		common_glue_func_t ecb;
		common_glue_cbc_func_t cbc;
		common_glue_ctr_func_t ctr;
		common_glue_xts_func_t xts;
	} fn_u;
};

struct common_glue_ctx {
	unsigned int num_funcs;
	int fpu_blocks_limit; 

	
	struct common_glue_func_entry funcs[];
};

static inline bool glue_fpu_begin(unsigned int bsize, int fpu_blocks_limit,
				  struct skcipher_walk *walk,
				  bool fpu_enabled, unsigned int nbytes)
{
	if (likely(fpu_blocks_limit < 0))
		return false;

	if (fpu_enabled)
		return true;

	
	if (nbytes < bsize * (unsigned int)fpu_blocks_limit)
		return false;

	
	skcipher_walk_atomise(walk);

	kernel_fpu_begin();
	return true;
}

static inline void glue_fpu_end(bool fpu_enabled)
{
	if (fpu_enabled)
		kernel_fpu_end();
}

static inline void le128_to_be128(be128 *dst, const le128 *src)
{
	dst->a = cpu_to_be64(le64_to_cpu(src->a));
	dst->b = cpu_to_be64(le64_to_cpu(src->b));
}

static inline void be128_to_le128(le128 *dst, const be128 *src)
{
	dst->a = cpu_to_le64(be64_to_cpu(src->a));
	dst->b = cpu_to_le64(be64_to_cpu(src->b));
}

static inline void le128_inc(le128 *i)
{
	u64 a = le64_to_cpu(i->a);
	u64 b = le64_to_cpu(i->b);

	b++;
	if (!b)
		a++;

	i->a = cpu_to_le64(a);
	i->b = cpu_to_le64(b);
}

extern int glue_ecb_req_128bit(const struct common_glue_ctx *gctx,
			       struct skcipher_request *req);

extern int glue_cbc_encrypt_req_128bit(const common_glue_func_t fn,
				       struct skcipher_request *req);

extern int glue_cbc_decrypt_req_128bit(const struct common_glue_ctx *gctx,
				       struct skcipher_request *req);

extern int glue_ctr_req_128bit(const struct common_glue_ctx *gctx,
			       struct skcipher_request *req);

extern int glue_xts_req_128bit(const struct common_glue_ctx *gctx,
			       struct skcipher_request *req,
			       common_glue_func_t tweak_fn, void *tweak_ctx,
			       void *crypt_ctx, bool decrypt);

extern void glue_xts_crypt_128bit_one(const void *ctx, u8 *dst,
				      const u8 *src, le128 *iv,
				      common_glue_func_t fn);

#endif 
