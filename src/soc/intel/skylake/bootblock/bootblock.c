/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corporation..
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <bootblock_common.h>
#include <fsp/bootblock.h>
#include <soc/bootblock.h>
#include <soc/romstage.h>

void asmlinkage bootblock_c_entry(uint64_t base_timestamp)
{
	/* Call lib/bootblock.c main */
	bootblock_main_with_timestamp(base_timestamp);
}

void bootblock_soc_early_init(void)
{
	bootblock_systemagent_early_init();
	bootblock_pch_early_init();
	bootblock_cpu_init();

	if (IS_ENABLED(CONFIG_BOOTBLOCK_CONSOLE))
		pch_uart_init();
}

void bootblock_soc_init(void)
{
	/* locate and call FspTempRamInit */
	bootblock_fsp_temp_ram_init();
	/*
	 * Perform early chipset initialization before fsp memory init
	 * example: pirq->irq programming, enabling smbus, pmcbase, abase,
	 * 			get platform info, i2c programming
	 */
	report_platform_info();
	set_max_freq();
	pch_early_init();
	i2c_early_init();
}
