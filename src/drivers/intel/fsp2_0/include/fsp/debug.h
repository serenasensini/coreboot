/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef _FSP2_0_DEBUG_H_
#define _FSP2_0_DEBUG_H_

#include <fsp/util.h>

/* FSP debug API */
void fsp_debug_before_memory_init(fsp_memory_init_fn memory_init,
	const struct FSPM_UPD *fspm_old_upd,
	const struct FSPM_UPD *fspm_new_upd);
void fsp_debug_after_memory_init(enum fsp_status status);
void fsp_debug_before_silicon_init(fsp_silicon_init_fn silicon_init,
	const struct FSPS_UPD *fsps_old_upd,
	const struct FSPS_UPD *fsps_new_upd);
void fsp_debug_after_silicon_init(enum fsp_status status);
void fsp_before_debug_notify(fsp_notify_fn notify,
	const struct fsp_notify_params *notify_params);
void fsp_debug_after_notify(enum fsp_status status);
void fspm_display_upd_values(const struct FSPM_UPD *old,
	const struct FSPM_UPD *new);
void fsp_display_hobs(void);
void fsp_verify_memory_init_hobs(void);
void fsp_print_header_info(const struct fsp_header *hdr);

/* Callbacks for displaying UPD parameters - place in a separate file
 * that is conditionally build with CONFIG_DISPLAY_UPD_DATA.
 */
void soc_display_fspm_upd_params(const struct FSPM_UPD *fspm_old_upd,
	const struct FSPM_UPD *fspm_new_upd);
void soc_display_fsps_upd_params(const struct FSPS_UPD *fsps_old_upd,
	const struct FSPS_UPD *fsps_new_upd);

/* Callbacks for displaying HOBs - place in a separate file that is
 * conditionally build with CONFIG_DISPLAY_HOBS.
 */
const char *soc_get_hob_type_name(const struct hob_header *hob);
const char *soc_get_guid_name(const uint8_t *guid);
void soc_display_hob(const struct hob_header *hob);

/* FSP debug utility functions */
void fsp_display_upd_value(const char *name, size_t size, uint64_t old,
	uint64_t new);
void fsp_print_guid(const void *guid);
void fsp_print_memory_resource_hobs(void);
void fsp_print_resource_descriptor(const void *base);
const char *fsp_get_hob_type_name(const struct hob_header *hob);
const char *fsp_get_guid_name(const uint8_t *guid);
void fsp_print_guid_extension_hob(const struct hob_header *hob);
int fsp_find_bootloader_tolum(struct range_entry *re);

#endif /* _FSP2_0_DEBUG_H_ */
