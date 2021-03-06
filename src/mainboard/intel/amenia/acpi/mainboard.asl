/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 * (Written by Lance Zhao <lijian.zhao@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <on_board.h>
#include "../ec.h"

Scope (\_SB)
{
	Device (LID0)
	{
		Name (_HID, EisaId ("PNP0C0D"))
		Method (_LID, 0)
		{
			Return (\_SB.PCI0.LPCB.EC0.LIDS)
		}
		Name (_PRW, Package () { GPE_EC_WAKE, 0x3 })
	}

	Device (PWRB)
	{
		Name (_HID, EisaId ("PNP0C0C"))
	}
}

Scope (\_SB.PCI0.I2C4)
{
	/* Standard Mode: HCNT, LCNT, SDA Hold Register */
	/* SDA Hold register value of 40 indicates
	 * sda hold time of 0.3us for ic_clk of 133MHz
	 */
	Name (SSCN, Package () { 0, 0, 40 })

	/* Fast Mode: HCNT, LCNT, SDA Hold Register */
	/* SDA Hold register value of 40 indicates
	 * sda hold time of 0.3us for ic_clk of 133MHz
	 */
	Name (FMCN, Package () { 0, 0, 40 })

	Device (ETPA)
	{
		Name (_HID, "ELAN0000")
		Name (_DDN, "Elan Touchpad")
		Name (_UID, 1)
		Name (ISTP, 1) /* Touchpad */

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				0x15,                     // SlaveAddress
				ControllerInitiated,      // SlaveMode
				400000,                   // ConnectionSpeed
				AddressingMode7Bit,       // AddressingMode
				"\\_SB.PCI0.I2C4",        // ResourceSource
			)
			Interrupt (ResourceConsumer, Edge, ActiveLow)
			{
				TOUCHPAD_INT
			}
		})

		Method (_STA)
		{
				Return (0xF)
		}

	}
}

Scope (\_SB.PCI0.I2C3)
{
	Device (ETSA)
	{
		Name (_HID, "ELAN0001")
		Name (_DDN, "Elan Touchscreen")
		Name (_UID, 1)
		Name (ISTP, 0) /* TouchScreen */
		Name (_S0W, 4)

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				0x10,                     // SlaveAddress
				ControllerInitiated,      // SlaveMode
				400000,                   // ConnectionSpeed
				AddressingMode7Bit,       // AddressingMode
				"\\_SB.PCI0.I2C3",        // ResourceSource
			)
			Interrupt (ResourceConsumer, Edge, ActiveLow)
			{
				GPIO_21_IRQ
			}
		})

		Method (_STA)
		{
			Return (0xF)
		}
	}
}

/*
 * LPC Trusted Platform Module
 */
Scope (\_SB.PCI0.LPCB)
{
	#include <drivers/pc80/tpm/acpi/tpm.asl>
	#include "ec.asl"
}
