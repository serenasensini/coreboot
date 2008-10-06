/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#include <types.h>
#include <lib.h>
#include <console.h>
#include <device/pci.h>
#include <msr.h>
#include <legacy.h>
#include <device/pci.h>
#include <statictree.h>
#include <config.h>
#include <io.h>
#include "rs690.h"

 
#define NBHTIU_INDEX		0xA8
#define NBMISC_INDEX		0x60
#define NBMC_INDEX 		0xE8

static u32 nb_bdf_read_index(u32 bdf, u32 index_reg, u32 index)
{
	pci_conf1_write_config32(bdf, index_reg, index);
	return pci_conf1_read_config32(bdf, index_reg + 0x4);
}

static void nb_bdf_write_index(u32 bdf, u32 index_reg, u32 index, u32 data)
{
	pci_conf1_write_config32(bdf, index_reg, index /* | 0x80 */ );
	pci_conf1_write_config32(bdf, index_reg + 0x4, data);
}

static u32 nbmisc_bdf_read_index(u32 nb_bdf, u32 index)
{
	return nb_bdf_read_index((nb_bdf), NBMISC_INDEX, (index));
}

static void nbmisc_bdf_write_index(u32 nb_bdf, u32 index, u32 data)
{
	nb_bdf_write_index((nb_bdf), NBMISC_INDEX, ((index) | 0x80), (data));
}

static u32 htiu_bdf_read_index(u32 nb_bdf, u32 index)
{
	return nb_bdf_read_index((nb_bdf), NBHTIU_INDEX, (index));
}

static void htiu_bdf_write_index(u32 nb_bdf, u32 index, u32 data)
{
	nb_bdf_write_index((nb_bdf), NBHTIU_INDEX, ((index) | 0x100), (data));
}

static u32 nbmc_bdf_read_index(u32 nb_bdf, u32 index)
{
	return nb_bdf_read_index((nb_bdf), NBMC_INDEX, (index));
}

static void nbmc_bdf_write_index(u32 nb_bdf, u32 index, u32 data)
{
	nb_bdf_write_index((nb_bdf), NBMC_INDEX, ((index) | 1 << 9), (data));
}

static void set_htiu_bdf_enable_bits(u32 nb_bdf, u32 reg_pos, u32 mask,
				 u32 val)
{
	u32 reg_old, reg;
	reg = reg_old = htiu_bdf_read_index(nb_bdf, reg_pos);
	reg &= ~mask;
	reg |= val;
	if (reg != reg_old) {
		htiu_bdf_write_index(nb_bdf, reg_pos, reg);
	}
}

static void set_nbmisc_bdf_enable_bits(u32 nb_bdf, u32 reg_pos, u32 mask,
				   u32 val)
{
	u32 reg_old, reg;
	reg = reg_old = nbmisc_bdf_read_index(nb_bdf, reg_pos);
	reg &= ~mask;
	reg |= val;
	if (reg != reg_old) {
		nbmisc_bdf_write_index(nb_bdf, reg_pos, reg);
	}
}

static void set_nbcfg_bdf_enable_bits(u32 nb_bdf, u32 reg_pos, u32 mask,
				  u32 val)
{
	u32 reg_old, reg;
	reg = reg_old = pci_conf1_read_config32(nb_bdf, reg_pos);
	reg &= ~mask;
	reg |= val;
	if (reg != reg_old) {
		pci_conf1_write_config32(nb_bdf, reg_pos, reg);
	}
}

static void set_nbcfg_bdf_enable_bits_8(u32 nb_bdf, u32 reg_pos, u8 mask,
				    u8 val)
{
	u8 reg_old, reg;
	reg = reg_old = pci_conf1_read_config8(nb_bdf, reg_pos);
	reg &= ~mask;
	reg |= val;
	if (reg != reg_old) {
		pci_conf1_write_config8(nb_bdf, reg_pos, reg);
	}
}

static void set_nbmc_bdf_enable_bits(u32 nb_bdf, u32 reg_pos, u32 mask,
				 u32 val)
{
	u32 reg_old, reg;
	reg = reg_old = nbmc_bdf_read_index(nb_bdf, reg_pos);
	reg &= ~mask;
	reg |= val;
	if (reg != reg_old) {
		nbmc_bdf_write_index(nb_bdf, reg_pos, reg);
	}
}

/*
* Compliant with CIM_33's ATINB_PrepareInit
*/
static void get_cpu_rev(void)
{
	u32 eax, ebx, ecx, edx;
	__asm__ volatile ("cpuid":"=a" (eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
			  :"0"(1));
	printk(BIOS_INFO, "get_cpu_rev EAX=0x%x.\n", eax);
	if (eax <= 0xfff)
		printk(BIOS_INFO, "CPU Rev is K8_Cx.\n");
	else if (eax <= 0x10fff)
		printk(BIOS_INFO, "CPU Rev is K8_Dx.\n");
	else if (eax <= 0x20fff)
		printk(BIOS_INFO, "CPU Rev is K8_Ex.\n");
	else if (eax <= 0x40fff)
		printk(BIOS_INFO, "CPU Rev is K8_Fx.\n");
	else if (eax == 0x60fb1 || eax == 0x60f81)	/*These two IDS are exception, they are G1. */
		printk(BIOS_INFO, "CPU Rev is K8_G1.\n");
	else if (eax <= 0X60FF0)
		printk(BIOS_INFO, "CPU Rev is K8_G0.\n");
	else if (eax <= 0x100000)
		printk(BIOS_INFO, "CPU Rev is K8_G1.\n");
	else
		printk(BIOS_INFO, "CPU Rev is K8_10.\n");
}

static u8 get_nb_bdf_rev(u32 nb_bdf)
{
	u32 reg;
	reg = pci_conf1_read_config32(nb_bdf, 0x00);
	if (0x7911 == (reg >> 16))
		return 7;
	reg = pci_conf1_read_config8(nb_bdf, 0x89);	/* copy from CIM, can't find in doc */
	if (reg & 0x2)		/* check bit1 */
		return 7;
	if (reg & 0x1)		/* check bit0 */
		return 6;
	else
		return 5;
}

/*****************************************
* Compliant with CIM_33's ATINB_HTInit
* Init HT link speed/width for rs690 -- k8 link
*****************************************/
static void rs690_htinit(void)
{
	/*
	 * About HT, it has been done in enumerate_ht_chain().
	 */
	u32 k8_f0;
	u32 reg;
	u8 k8_ht_freq;

	k8_f0 = PCI_BDF(0, 0x18, 0);
	/************************
	* get k8's ht freq, in k8's function 0, offset 0x88
	* bit11-8, specifics the maximum operation frequency of the link's transmitter clock.
	* The link frequency field (Frq) is cleared by cold reset. SW can write a nonzero
	* value to this reg, and that value takes effect on the next warm reset or 
	* LDTSTOP_L disconnect sequence.
	* 0000b = 200Mhz
	* 0010b = 400Mhz
	* 0100b = 600Mhz
	* 0101b = 800Mhz
	* 0110b = 1Ghz
	* 1111b = 1Ghz
	************************/
	reg = pci_conf1_read_config32(k8_f0, 0x88);
	k8_ht_freq = (reg & 0xf00) >> 8;
	printk(BIOS_INFO, "rs690_ht_init k8_ht_freq=%x.\n", k8_ht_freq);
}

/*******************************************************
* Optimize k8 with UMA.
* See BKDG_NPT_0F guide for details.
* The processor node is addressed by its Node ID on the HT link and can be
* accessed with a bdfice number in the PCI configuration space on Bus0.
* The Node ID 0 is mapped to Device 24 (0x18), the Node ID 1 is mapped
* to Device 25, and so on.
* The processor implements configuration registers in PCI configuration
* space using the following four headers
*	Function0: HT technology configuration
*	Function1: Address map configuration
*	Function2: DRAM and HT technology Trace mode configuration
*	Function3: Miscellaneous configuration
*******************************************************/
static void k8_optimization(void)
{
	u32 k8_f0, k8_f2, k8_f3;
	struct msr msr;

	printk(BIOS_INFO, "k8_optimization()\n");
	k8_f0 = PCI_BDF(0, 0x18, 0);
	k8_f2 = PCI_BDF(0, 0x18, 2);
	k8_f3 = PCI_BDF(0, 0x18, 3);

	pci_conf1_write_config32(k8_f0, 0x90, 0x01700178);	/* CIM NPT_Optimization */
	set_nbcfg_bdf_enable_bits(k8_f0, 0x68, 1 << 28, 0 << 28);
	set_nbcfg_bdf_enable_bits(k8_f0, 0x68, 1 << 26 | 1 << 27,
			      1 << 26 | 1 << 27);
	set_nbcfg_bdf_enable_bits(k8_f0, 0x68, 1 << 11, 1 << 11);
	set_nbcfg_bdf_enable_bits(k8_f0, 0x84, 1 << 11 | 1 << 13 | 1 << 15, 1 << 11 | 1 << 13 | 1 << 15);	/* TODO */

	pci_conf1_write_config32(k8_f3, 0x70, 0x51320111);	/* CIM NPT_Optimization */
	pci_conf1_write_config32(k8_f3, 0x74, 0x50304021);
	pci_conf1_write_config32(k8_f3, 0x78, 0x08002A00);
	if (pci_conf1_read_config32(k8_f3, 0xE8) & 0x3<<12)
		pci_conf1_write_config32(k8_f3, 0x7C, 0x0000211B); /* dual core */
	else
		pci_conf1_write_config32(k8_f3, 0x7C, 0x0000211C); /* single core */
	set_nbcfg_bdf_enable_bits_8(k8_f3, 0xDC, 0xFF, 0x25);

	set_nbcfg_bdf_enable_bits(k8_f2, 0xA0, 1 << 5, 1 << 5);
	set_nbcfg_bdf_enable_bits(k8_f2, 0x94, 0xF << 24, 7 << 24);
	set_nbcfg_bdf_enable_bits(k8_f2, 0x90, 1 << 10, 1 << 10);
	set_nbcfg_bdf_enable_bits(k8_f2, 0xA0, 3 << 2, 3 << 2);
	set_nbcfg_bdf_enable_bits(k8_f2, 0xA0, 1 << 5, 1 << 5);

	msr = rdmsr(0xC001001F);
	msr.lo &= ~(1 << 9);
	msr.hi &= ~(1 << 4);
	wrmsr(0xC001001F, msr);
}

/*****************************************
* Compliant with CIM_33's ATINB_PCICFG_POR_TABLE
*****************************************/
static void rs690_por_pcicfg_init(u32 nb_bdf)
{
	/* enable PCI Memory Access */
	set_nbcfg_bdf_enable_bits_8(nb_bdf, 0x04, (u8)(~0xFD), 0x02);
	/* Set RCRB Enable */
	set_nbcfg_bdf_enable_bits_8(nb_bdf, 0x84, (u8)(~0xFF), 0x1);
	/* allow decode of 640k-1MB */
	set_nbcfg_bdf_enable_bits_8(nb_bdf, 0x84, (u8)(~0xEF), 0x10);
	/* Enable PM2_CNTL(BAR2) IO mapped cfg write access to be broadcast to both NB and SB */
	set_nbcfg_bdf_enable_bits_8(nb_bdf, 0x84, (u8)(~0xFF), 0x4);
	/* Power Management Register Enable */
	set_nbcfg_bdf_enable_bits_8(nb_bdf, 0x84, (u8)(~0xFF), 0x80);

	/* Reg4Ch[1]=1 (APIC_ENABLE) force cpu request with address 0xFECx_xxxx to south-bridge
	 * Reg4Ch[6]=1 (BMMsgEn) enable BM_Set message generation
	 * BMMsgEn */
	set_nbcfg_bdf_enable_bits_8(nb_bdf, 0x4C, (u8)(~0x00), 0x42 | 1);

	/* Reg4Ch[16]=1 (WakeC2En) enable Wake_from_C2 message generation.
	 * Reg4Ch[18]=1 (P4IntEnable) Enable north-bridge to accept MSI with address 0xFEEx_xxxx from south-bridge */
	set_nbcfg_bdf_enable_bits_8(nb_bdf, 0x4E, (u8)(~0xFF), 0x05);
	/* Reg94h[4:0] = 0x0  P drive strength offset 0
	 * Reg94h[6:5] = 0x2  P drive strength additive adjust */
	set_nbcfg_bdf_enable_bits_8(nb_bdf, 0x94, (u8)(~0x80), 0x40);

	/* Reg94h[20:16] = 0x0  N drive strength offset 0
	 * Reg94h[22:21] = 0x2  N drive strength additive adjust */
	set_nbcfg_bdf_enable_bits_8(nb_bdf, 0x96, (u8)(~0x80), 0x40);

	/* Reg80h[4:0] = 0x0  Termination offset
	 * Reg80h[6:5] = 0x2  Termination additive adjust */
	set_nbcfg_bdf_enable_bits_8(nb_bdf, 0x80, (u8)(~0x80), 0x40);

	/* Reg80h[14] = 0x1   Enable receiver termination control */
	set_nbcfg_bdf_enable_bits_8(nb_bdf, 0x81, (u8)(~0xFF), 0x40);

	/* Reg94h[15] = 0x1 Enables HT transmitter advanced features to be turned on
	 * Reg94h[14] = 0x1  Enable drive strength control */
	set_nbcfg_bdf_enable_bits_8(nb_bdf, 0x95, (u8)(~0x3F), 0xC4);

	/* Reg94h[31:29] = 0x7 Enables HT transmitter de-emphasis */
	set_nbcfg_bdf_enable_bits_8(nb_bdf, 0x97, (u8)(~0x1F), 0xE0);

	/*Reg8Ch[10:9] = 0x3 Enables Gfx Debug BAR,
	 * force this BAR as mem type in rs690_gfx.c */
	set_nbcfg_bdf_enable_bits_8(nb_bdf, 0x8D, (u8)(~0xFF), 0x03);
	
}

/*****************************************
* Compliant with CIM_33's ATINB_MCIndex_POR_TABLE
*****************************************/
static void rs690_por_mc_index_init(u32 nb_bdf)
{
	set_nbmc_bdf_enable_bits(nb_bdf, 0x7A, ~0xFFFFFF80, 0x0000005F);
	set_nbmc_bdf_enable_bits(nb_bdf, 0xD8, ~0x00000000, 0x00600060);
	set_nbmc_bdf_enable_bits(nb_bdf, 0xD9, ~0x00000000, 0x00600060);
	set_nbmc_bdf_enable_bits(nb_bdf, 0xE0, ~0x00000000, 0x00000000);
	set_nbmc_bdf_enable_bits(nb_bdf, 0xE1, ~0x00000000, 0x00000000);
	set_nbmc_bdf_enable_bits(nb_bdf, 0xE8, ~0x00000000, 0x003E003E);
	set_nbmc_bdf_enable_bits(nb_bdf, 0xE9, ~0x00000000, 0x003E003E);
}

/*****************************************
* Compliant with CIM_33's ATINB_MISCIND_POR_TABLE
* Compliant with CIM_33's MISC_INIT_TBL
*****************************************/
static void rs690_por_misc_index_init(u32 nb_bdf)
{
	/* NB_MISC_IND_WR_EN + IOC_PCIE_CNTL
	 * Block non-snoop DMA request if PMArbDis is set.
	 * Set BMSetDis */
	set_nbmisc_bdf_enable_bits(nb_bdf, 0x0B, ~0xFFFF0000, 0x00000180);
	set_nbmisc_bdf_enable_bits(nb_bdf, 0x01, ~0xFFFFFFFF, 0x00000040);

	/* NBCFG (NBMISCIND 0x0): NB_CNTL -
	 *   HIDE_NB_AGP_CAP  ([0], default=1)HIDE
	 *   HIDE_P2P_AGP_CAP ([1], default=1)HIDE
	 *   HIDE_NB_GART_BAR ([2], default=1)HIDE
	 *   AGPMODE30        ([4], default=0)DISABLE
	 *   AGP30ENCHANCED   ([5], default=0)DISABLE
	 *   HIDE_AGP_CAP     ([8], default=1)ENABLE */
	set_nbmisc_bdf_enable_bits(nb_bdf, 0x00, ~0xFFFF0000, 0x00000506);	/* set bit 10 for MSI */

	/* NBMISCIND:0x6A[16]= 1 SB link can get a full swing
	 *      set_nbmisc_bdf_enable_bits(nb_bdf, 0x6A, 0ffffffffh, 000010000);
	 * NBMISCIND:0x6A[17]=1 Set CMGOOD_OVERRIDE. */
	set_nbmisc_bdf_enable_bits(nb_bdf, 0x6A, ~0xffffffff, 0x00020000);

	/* NBMISIND:0x40 Bit[8]=1 and Bit[10]=1 following bits are required to set in order to allow LVDS or PWM features to work. */
	set_nbmisc_bdf_enable_bits(nb_bdf, 0x40, ~0xffffffff, 0x00000500);

	/* NBMISIND:0xC Bit[13]=1 Enable GSM mode for C1e or C3 with pop-up. */
	set_nbmisc_bdf_enable_bits(nb_bdf, 0x0C, ~0xffffffff, 0x00002000);

	/* Set NBMISIND:0x1F[3] to map NB F2 interrupt pin to INTB# */
	set_nbmisc_bdf_enable_bits(nb_bdf, 0x1F, ~0xffffffff, 0x00000008);

	/* Compliant with CIM_33's MISC_INIT_TBL, except Hide NB_BAR3_PCIE
	 * Enable access to DEV8
	 * Enable setPower message for all ports
	 */
	set_nbmisc_bdf_enable_bits(nb_bdf, 0x00, 1 << 6, 1 << 6);
	set_nbmisc_bdf_enable_bits(nb_bdf, 0x0b, 1 << 20, 1 << 20);
	set_nbmisc_bdf_enable_bits(nb_bdf, 0x51, 1 << 20, 1 << 20);
	set_nbmisc_bdf_enable_bits(nb_bdf, 0x53, 1 << 20, 1 << 20);
	set_nbmisc_bdf_enable_bits(nb_bdf, 0x55, 1 << 20, 1 << 20);
	set_nbmisc_bdf_enable_bits(nb_bdf, 0x57, 1 << 20, 1 << 20);
	set_nbmisc_bdf_enable_bits(nb_bdf, 0x59, 1 << 20, 1 << 20);
	set_nbmisc_bdf_enable_bits(nb_bdf, 0x5B, 1 << 20, 1 << 20);

	set_nbmisc_bdf_enable_bits(nb_bdf, 0x00, 1 << 7, 1 << 7);
	set_nbmisc_bdf_enable_bits(nb_bdf, 0x07, 0x000000f0, 0x30);
	/* Disable bus-master trigger event from SB and Enable set_slot_power message to SB */
	set_nbmisc_bdf_enable_bits(nb_bdf, 0x0B, 0xffffffff, 0x500180);
}

/*****************************************
* Compliant with CIM_33's ATINB_HTIUNBIND_POR_TABLE
*****************************************/
static void rs690_por_htiu_index_init(u32 nb_bdf)
{
	/* 0xBC:
	* Enables GSM mode for C1e or C3 with pop-up
	* Prevents AllowLdtStop from being asserted during HT link recovery
	* Allows FID cycles to be serviced faster. Needed for RS690 A12. No harm in RS690 A11 */
	set_htiu_bdf_enable_bits(nb_bdf, 0x05, ~0xffffffff, 0x0BC);
	/* 0x4203A202:
	* Enables writes to pass in-progress reads
	* Enables streaming of CPU writes
	* Enables extended write buffer for CPU writes
	* Enables additional response buffers
	* Enables special reads to pass writes
	* Enables decoding of C1e/C3 and FID cycles
	* Enables HTIU-display handshake bypass.
	* Enables tagging fix */
	set_htiu_bdf_enable_bits(nb_bdf, 0x06, ~0xFFFFFFFE, 0x4203A202);

	/* Enables byte-write optimization for IOC requests
	* Disables delaying STPCLK de-assert during FID sequence. Needed when enhanced UMA arbitration is used.
	* Disables upstream system-management delay */
	set_htiu_bdf_enable_bits(nb_bdf, 0x07, ~0xFFFFFFF9, 0x001);

	/* HTIUNBIND 0x16 [1] = 0x1     Enable crc decoding fix */
	set_htiu_bdf_enable_bits(nb_bdf, 0x16, ~0xFFFFFFFF, 0x2);
}

/*****************************************
* Compliant with CIM_33's ATINB_POR_INIT_JMPDI
* Configure RS690 registers to power-on default RPR.
* POR: Power On Reset
* RPR: Register Programming Requirements
*****************************************/
static void rs690_por_init(u32 nb_bdf)
{
	printk(BIOS_INFO, "rs690_por_init\n");
	/* ATINB_PCICFG_POR_TABLE, initialize the values for rs690 PCI Config registers */
	rs690_por_pcicfg_init(nb_bdf);

	/* ATINB_MCIND_POR_TABLE */
	rs690_por_mc_index_init(nb_bdf);

	/* ATINB_MISCIND_POR_TABLE */
	rs690_por_misc_index_init(nb_bdf);

	/* ATINB_HTIUNBIND_POR_TABLE */
	rs690_por_htiu_index_init(nb_bdf);

	/* ATINB_CLKCFG_PORT_TABLE */
	/* rs690 A11 SB Link full swing? */
}

/* enable CFG access to Dev8, which is the SB P2P Bridge */
void enable_rs690_dev8(void)
{
	set_nbmisc_bdf_enable_bits(PCI_BDF(0, 0, 0), 0x00, 1 << 6, 1 << 6);
}



/*
* Compliant with CIM_33's AtiNBInitEarlyPost (AtiInitNBBeforePCIInit).
*/
void rs690_before_pci_init(void)
{
}

/*
* The calling sequence is same as CIM.
*/
void rs690_stage1(void)
{
	u32 nb_bdf = PCI_BDF(0, 0, 0);
	printk(BIOS_INFO, "rs690_early_setup()\n");

	/*ATINB_PrepareInit */
	get_cpu_rev();
	switch (get_nb_bdf_rev(nb_bdf)) {	/* PCIEMiscInit */
	case 5:
		printk(BIOS_INFO, "NB Revision is A11.\n");
		break;
	case 6:
		printk(BIOS_INFO, "NB Revision is A12.\n");
		break;
	case 7:
		printk(BIOS_INFO, "NB Revision is A21.\n");
		break;
	}

	rs690_htinit();
	k8_optimization();
	rs690_por_init(nb_bdf);
}
