/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright 2018 Oleksij Rempel <o.rempel@pengutronix.de>, Pengutronix
 */

#define BOARD_PBL_START start_dlink_dir_320

#include <asm/asm.h>
#include <asm/pbl_macros.h>
#include <mach/debug_ll.h>
#include <asm/pbl_nmon.h>
#include <linux/sizes.h>

ENTRY_FUNCTION(BOARD_PBL_START)

	mips_cpu_setup

	/* CPU/SoC specific setup ... */
	/* ... absent */

ENTRY_FUNCTION_END(BOARD_PBL_START, dlink_dir_320, SZ_32M)
