/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2016 Intel Corp.
 * (Written by Andrey Petrov <andrey.petrov@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <arch/cpu.h>
#include <cbfs.h>
#include <cbmem.h>
#include <commonlib/fsp.h>
#include <console/console.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <program_loading.h>
#include <string.h>
#include <timestamp.h>

struct fsp_header fsps_hdr;

static void do_silicon_init(struct fsp_header *hdr)
{
	struct FSPS_UPD upd, *supd;
	fsp_silicon_init_fn silicon_init;
	enum fsp_status status;

	supd = (struct FSPS_UPD *) (hdr->cfg_region_offset + hdr->image_base);

	if (supd->FspUpdHeader.Signature != FSPS_UPD_SIGNATURE) {
		die("Invalid FSPS signature\n");
	}

	memcpy(&upd, supd, sizeof(upd));

	/* Give SoC/mainboard a chance to populate entries */
	platform_fsp_silicon_init_params_cb(&upd);

	/* Call SiliconInit */
	silicon_init = (void *) (hdr->image_base +
				 hdr->silicon_init_entry_offset);
	fsp_debug_before_silicon_init(silicon_init, supd, &upd);

	timestamp_add_now(TS_FSP_SILICON_INIT_START);
	post_code(POST_FSP_SILICON_INIT);
	status = silicon_init(&upd);
	timestamp_add_now(TS_FSP_SILICON_INIT_END);
	post_code(POST_FSP_SILICON_INIT);

	fsp_debug_after_silicon_init(status);

	/* Handle any errors returned by FspSiliconInit */
	fsp_handle_reset(status);
	if (status != FSP_SUCCESS) {
		printk(BIOS_SPEW, "FspSiliconInit returned 0x%08x\n", status);
		die("FspSiliconINit returned an error!\n");
	}
}

void fsp_silicon_init(void)
{
	struct fsp_header *hdr = &fsps_hdr;
	struct cbfsf file_desc;
	struct region_device rdev;
	const char *name = CONFIG_FSP_S_CBFS;
	void *dest;
	size_t size;

	if (cbfs_boot_locate(&file_desc, name, NULL)) {
		printk(BIOS_ERR, "Could not locate %s in CBFS\n", name);
		die("FSPS not available!\n");
	}

	cbfs_file_data(&rdev, &file_desc);

	/* Load and relocate into CBMEM. */
	size = region_device_sz(&rdev);
	dest = cbmem_add(CBMEM_ID_REFCODE, size);

	if (dest == NULL) {
		die("Could not add FSPS to CBMEM!\n");
	}

	if (rdev_readat(&rdev, dest, 0, size) < 0)
		die("Failed to read FSPS!\n");

	if (fsp_component_relocate((uintptr_t)dest, dest, size) < 0) {
		die("Unable to relocate FSPS!\n");
	}

	/* Create new region device in memory after relocation. */
	rdev_chain(&rdev, &addrspace_32bit.rdev, (uintptr_t)dest, size);

	if (fsp_validate_component(hdr, &rdev) != CB_SUCCESS)
		die("Invalid FSPS header!\n");

	/* Signal that FSP component has been loaded. */
	prog_segment_loaded(hdr->image_base, hdr->image_size, SEG_FINAL);

	do_silicon_init(hdr);
}
