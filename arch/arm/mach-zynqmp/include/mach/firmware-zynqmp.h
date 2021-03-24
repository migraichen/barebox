/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Xilinx Zynq MPSoC Firmware layer
 *
 * Copyright (c) 2018 Thomas Haemmerle <thomas.haemmerle@wolfvision.net>
 *
 * based on Linux xlnx-zynqmp
 *
 *  Michal Simek <michal.simek@xilinx.com>
 *  Davorin Mista <davorin.mista@aggios.com>
 *  Jolly Shah <jollys@xilinx.com>
 *  Rajan Vaja <rajanv@xilinx.com>
 */

#ifndef FIRMWARE_ZYNQMP_H_
#define FIRMWARE_ZYNQMP_H_

#include <firmware.h>

#define PAYLOAD_ARG_CNT			4

#define ZYNQMP_PM_VERSION(MAJOR, MINOR)	((MAJOR << 16) | MINOR)

#define ZYNQMP_FPGA_BIT_AUTH_DDR	BIT(1)
#define ZYNQMP_FPGA_BIT_AUTH_OCM	BIT(2)
#define ZYNQMP_FPGA_BIT_ENC_USR_KEY	BIT(3)
#define ZYNQMP_FPGA_BIT_ENC_DEV_KEY	BIT(4)
#define ZYNQMP_FPGA_BIT_ONLY_BIN	BIT(5)

#define ZYNQMP_PCAP_STATUS_FPGA_DONE	BIT(3)

#define ZYNQMP_PM_FEATURE_BYTE_ORDER_IRREL	BIT(0)
#define ZYNQMP_PM_FEATURE_SIZE_NOT_NEEDED	BIT(1)

#define ZYNQMP_PM_VERSION_1_0_FEATURES	0
#define ZYNQMP_PM_VERSION_1_1_FEATURES	(ZYNQMP_PM_FEATURE_BYTE_ORDER_IRREL | \
					 ZYNQMP_PM_FEATURE_SIZE_NOT_NEEDED)

/*
 * Xilinx KU040 Bitstream Composition:
 *
 * Bitstream can be provided with an optinal header (`struct bs_header`).
 * The true bitstream starts with the binary-header composed of 21 words:
 *
 *  0: 0xFFFFFFFF (Dummy pad word)
 *     ...
 * 15: 0xFFFFFFFF (Dummy pad word)
 * 16: 0x000000BB (Bus width auto detect word 1)
 * 17: 0x11220044 (Bus width auto detect word 2)
 * 18: 0xFFFFFFFF (Dummy pad word)
 * 19: 0xFFFFFFFF (Dummy pad word)
 * 20: 0xAA995566 (Sync word)
 *
 * See Xilinx UG570 (v1.11) September 30 2019, Chapter 9 "Configuration
 * Details - Bitstream Composition" for further details.
 */
#define DUMMY_WORD			0xFFFFFFFF
#define BUS_WIDTH_AUTO_DETECT1_OFFSET	16
#define BUS_WIDTH_AUTO_DETECT1		0x000000BB
#define BUS_WIDTH_AUTO_DETECT2_OFFSET	17
#define BUS_WIDTH_AUTO_DETECT2		0x11220044
#define SYNC_WORD_OFFSET		20
#define SYNC_WORD			0xAA995566
#define BIN_HEADER_LENGTH		21

struct fpgamgr {
	struct firmware_handler fh;
	struct device_d dev;
	const struct zynqmp_eemi_ops *eemi_ops;
	int programmed;
	char *buf;
	size_t size;
	u32 features;
};

enum pm_ioctl_id {
	IOCTL_SET_PLL_FRAC_MODE = 8,
	IOCTL_GET_PLL_FRAC_MODE,
	IOCTL_SET_PLL_FRAC_DATA,
	IOCTL_GET_PLL_FRAC_DATA,
};

enum pm_query_id {
	PM_QID_INVALID,
	PM_QID_CLOCK_GET_NAME,
	PM_QID_CLOCK_GET_TOPOLOGY,
	PM_QID_CLOCK_GET_FIXEDFACTOR_PARAMS,
	PM_QID_CLOCK_GET_PARENTS,
	PM_QID_CLOCK_GET_ATTRIBUTES,
	PM_QID_CLOCK_GET_NUM_CLOCKS = 12,
};

/**
 * struct zynqmp_pm_query_data - PM query data
 * @qid:	query ID
 * @arg1:	Argument 1 of query data
 * @arg2:	Argument 2 of query data
 * @arg3:	Argument 3 of query data
 */
struct zynqmp_pm_query_data {
	u32 qid;
	u32 arg1;
	u32 arg2;
	u32 arg3;
};

struct zynqmp_eemi_ops {
	int (*get_api_version)(u32 *version);
	int (*query_data)(struct zynqmp_pm_query_data qdata, u32 *out);
	int (*clock_enable)(u32 clock_id);
	int (*clock_disable)(u32 clock_id);
	int (*clock_getstate)(u32 clock_id, u32 *state);
	int (*clock_setdivider)(u32 clock_id, u32 divider);
	int (*clock_getdivider)(u32 clock_id, u32 *divider);
	int (*clock_setrate)(u32 clock_id, u64 rate);
	int (*clock_getrate)(u32 clock_id, u64 *rate);
	int (*clock_setparent)(u32 clock_id, u32 parent_id);
	int (*clock_getparent)(u32 clock_id, u32 *parent_id);
	int (*ioctl)(u32 node_id, u32 ioctl_id, u32 arg1, u32 arg2, u32 *out);
	int (*fpga_getstatus)(u32 *status);
	int (*fpga_load)(u64 address, u32 size, u32 flags);
};

const struct zynqmp_eemi_ops *zynqmp_pm_get_eemi_ops(void);

#endif /* FIRMWARE_ZYNQMP_H_ */
