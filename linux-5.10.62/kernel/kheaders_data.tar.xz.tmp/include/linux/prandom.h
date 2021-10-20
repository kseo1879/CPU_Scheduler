/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _LINUX_PRANDOM_H
#define _LINUX_PRANDOM_H

#include <linux/types.h>
#include <linux/percpu.h>

u32 prandom_u32(void);
void prandom_bytes(void *buf, size_t nbytes);
void prandom_seed(u32 seed);
void prandom_reseed_late(void);

DECLARE_PER_CPU(unsigned long, net_rand_noise);

#define PRANDOM_ADD_NOISE(a, b, c, d) \
	prandom_u32_add_noise((unsigned long)(a), (unsigned long)(b), \
			      (unsigned long)(c), (unsigned long)(d))

#if BITS_PER_LONG == 64

#define PRND_SIPROUND(v0, v1, v2, v3) ( \
	v0 += v1, v1 = rol64(v1, 13),  v2 += v3, v3 = rol64(v3, 16), \
	v1 ^= v0, v0 = rol64(v0, 32),  v3 ^= v2,                     \
	v0 += v3, v3 = rol64(v3, 21),  v2 += v1, v1 = rol64(v1, 17), \
	v3 ^= v0,                      v1 ^= v2, v2 = rol64(v2, 32)  \
)

#define PRND_K0 (0x736f6d6570736575 ^ 0x6c7967656e657261)
#define PRND_K1 (0x646f72616e646f6d ^ 0x7465646279746573)

#elif BITS_PER_LONG == 32

#define PRND_SIPROUND(v0, v1, v2, v3) ( \
	v0 += v1, v1 = rol32(v1,  5),  v2 += v3, v3 = rol32(v3,  8), \
	v1 ^= v0, v0 = rol32(v0, 16),  v3 ^= v2,                     \
	v0 += v3, v3 = rol32(v3,  7),  v2 += v1, v1 = rol32(v1, 13), \
	v3 ^= v0,                      v1 ^= v2, v2 = rol32(v2, 16)  \
)
#define PRND_K0 0x6c796765
#define PRND_K1 0x74656462

#else
#error Unsupported BITS_PER_LONG
#endif

static inline void prandom_u32_add_noise(unsigned long a, unsigned long b,
					 unsigned long c, unsigned long d)
{
	
	a ^= raw_cpu_read(net_rand_noise);
	PRND_SIPROUND(a, b, c, d);
	raw_cpu_write(net_rand_noise, d);
}

struct rnd_state {
	__u32 s1, s2, s3, s4;
};

u32 prandom_u32_state(struct rnd_state *state);
void prandom_bytes_state(struct rnd_state *state, void *buf, size_t nbytes);
void prandom_seed_full_state(struct rnd_state __percpu *pcpu_state);

#define prandom_init_once(pcpu_state)			\
	DO_ONCE(prandom_seed_full_state, (pcpu_state))


static inline u32 prandom_u32_max(u32 ep_ro)
{
	return (u32)(((u64) prandom_u32() * ep_ro) >> 32);
}


static inline u32 __seed(u32 x, u32 m)
{
	return (x < m) ? x + m : x;
}


static inline void prandom_seed_state(struct rnd_state *state, u64 seed)
{
	u32 i = ((seed >> 32) ^ (seed << 10) ^ seed) & 0xffffffffUL;

	state->s1 = __seed(i,   2U);
	state->s2 = __seed(i,   8U);
	state->s3 = __seed(i,  16U);
	state->s4 = __seed(i, 128U);
	PRANDOM_ADD_NOISE(state, i, 0, 0);
}


static inline u32 next_pseudo_random32(u32 seed)
{
	return seed * 1664525 + 1013904223;
}

#endif
