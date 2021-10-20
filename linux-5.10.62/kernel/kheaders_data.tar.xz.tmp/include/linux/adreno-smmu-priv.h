// SPDX-License-Identifier: GPL-2.0-only


#ifndef __ADRENO_SMMU_PRIV_H
#define __ADRENO_SMMU_PRIV_H

#include <linux/io-pgtable.h>


struct adreno_smmu_priv {
    const void *cookie;
    const struct io_pgtable_cfg *(*get_ttbr1_cfg)(const void *cookie);
    int (*set_ttbr0_cfg)(const void *cookie, const struct io_pgtable_cfg *cfg);
};

#endif 