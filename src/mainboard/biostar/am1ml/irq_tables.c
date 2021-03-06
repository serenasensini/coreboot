/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Sergej Ivanov <getinaks@gmail.com>
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


#include <arch/pirq_routing.h>

const struct irq_routing_table intel_irq_routing_table = {
	PIRQ_SIGNATURE,		/* u32 signature */
	PIRQ_VERSION,		/* u16 version */
	32 + 16 * 10,		/* Max. number of devices on the bus */
	0x00,			/* Interrupt router bus */
	(0x14 << 3) | 0x3,	/* Interrupt router dev */
	0,			/* IRQs devoted exclusively to PCI usage */
	0x1002,			/* Vendor */
	0x439d,			/* Device */
	0,			/* Miniport */
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* u8 rfu[11] */
	0xa6,			/* Checksum (has to be set to some value that
				 * would give 0 after the sum of all bytes
				 * for this structure (including checksum).
                                 */
	{
		/* bus,        dev | fn,   {link, bitmap}, {link, bitmap}, {link, bitmap}, {link, bitmap}, slot, rfu */
		{0x00, (0x00 << 3) | 0x0, {{0x01, 0xccb0}, {0x02, 0xccb0}, {0x03, 0xccb0}, {0x04, 0xdab8}}, 0x0, 0x0},
		{0x00, (0x01 << 3) | 0x0, {{0x05, 0xccb0}, {0x06, 0xccb0}, {0x07, 0xccb0}, {0x08, 0xccb0}}, 0x0, 0x0},
		{0x00, (0x02 << 3) | 0x0, {{0x05, 0xccb0}, {0x06, 0xccb0}, {0x07, 0xccb0}, {0x08, 0xccb0}}, 0x0, 0x0},
		{0x00, (0x14 << 3) | 0x0, {{0x01, 0xccb0}, {0x02, 0xccb0}, {0x03, 0xccb0}, {0x04, 0xdab8}}, 0x0, 0x0},
		{0x00, (0x12 << 3) | 0x0, {{0x03, 0xccb0}, {0x02, 0xccb0}, {0x00, 0x0000}, {0x00, 0x0000}}, 0x0, 0x0},
		{0x00, (0x13 << 3) | 0x0, {{0x03, 0xccb0}, {0x02, 0xccb0}, {0x00, 0x0000}, {0x00, 0x0000}}, 0x0, 0x0},
		{0x00, (0x16 << 3) | 0x0, {{0x03, 0xccb0}, {0x02, 0xccb0}, {0x00, 0x0000}, {0x00, 0x0000}}, 0x0, 0x0},
		{0x00, (0x10 << 3) | 0x0, {{0x03, 0xccb0}, {0x00, 0x0000}, {0x00, 0x0000}, {0x00, 0x0000}}, 0x0, 0x0},
		{0x00, (0x11 << 3) | 0x0, {{0x04, 0xdab8}, {0x00, 0x0000}, {0x00, 0x0000}, {0x00, 0x0000}}, 0x0, 0x0},
		{0x01, (0x00 << 3) | 0x0, {{0x05, 0xccb0}, {0x06, 0xccb0}, {0x07, 0xccb0}, {0x08, 0xccb0}}, 0x12, 0x0},
	}
};

unsigned long write_pirq_routing_table(unsigned long addr)
{
	return copy_pirq_routing_table(addr, &intel_irq_routing_table);
}
