/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_EFI_EMBEDDED_FW_H
#define _LINUX_EFI_EMBEDDED_FW_H

#include <linux/list.h>
#include <linux/mod_devicetable.h>

#define EFI_EMBEDDED_FW_PREFIX_LEN		8


struct efi_embedded_fw {
	struct list_head list;
	const char *name;
	const u8 *data;
	size_t length;
};


struct efi_embedded_fw_desc {
	const char *name;
	u8 prefix[EFI_EMBEDDED_FW_PREFIX_LEN];
	u32 length;
	u8 sha256[32];
};

extern const struct dmi_system_id touchscreen_dmi_table[];

int efi_get_embedded_fw(const char *name, const u8 **dat, size_t *sz);

#endif
