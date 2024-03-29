/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (C) 2017 Oleksij Rempel <o.rempel@pengutronix.de>
 */

#define BOARD_PBL_START start_tplink_wdr4300

#include <mach/debug_ll.h>
#include <asm/asm.h>
#include <asm/pbl_macros.h>
#include <mach/pbl_macros.h>
#include <mach/pbl_ll_init_ar9344_1.1.h>
#include <asm/pbl_nmon.h>
#include <linux/sizes.h>

ENTRY_FUNCTION(BOARD_PBL_START)

	mips_cpu_setup

	debug_ll_ath79_init

	hornet_mips24k_cp0_setup

	/* test if we are in the SRAM */
	pbl_blt 0xbd000000 1f t8
	b skip_flash_test
	nop
1:
	/* test if we are in the flash */
	pbl_blt 0xbf000000 skip_pll_ram_config t8
skip_flash_test:

	pbl_ar9344_v11_pll_config

	pbl_ar9344_v11_ddr2_config

skip_pll_ram_config:

ENTRY_FUNCTION_END(BOARD_PBL_START, ar9344_tl_wdr4300_v1.7, SZ_128M)
