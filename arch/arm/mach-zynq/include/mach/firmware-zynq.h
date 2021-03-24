/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Xilinx Zynq Firmware layer
 *
 */

#ifndef FIRMWARE_ZYNQ_H_
#define FIRMWARE_ZYNQ_H_

#include <firmware.h>

#define CTRL_OFFSET				0x00
#define LOCK_OFFSET				0x04
#define INT_STS_OFFSET				0x0c
#define INT_MASK_OFFSET			0x10
#define STATUS_OFFSET				0x14
#define DMA_SRC_ADDR_OFFSET			0x18
#define DMA_DST_ADDR_OFFSET			0x1c
#define DMA_SRC_LEN_OFFSET			0x20
#define DMA_DEST_LEN_OFFSET			0x24
#define UNLOCK_OFFSET				0x34
#define MCTRL_OFFSET				0x80

#define CTRL_PCFG_PROG_B_MASK			BIT(30)
#define CTRL_PCAP_PR_MASK			BIT(27)
#define CTRL_PCAP_MODE_MASK			BIT(26)
#define CTRL_PCAP_RATE_EN_MASK		BIT(25)

#define STATUS_DMA_CMD_Q_F_MASK		BIT(31)
#define STATUS_PCFG_INIT_MASK			BIT(4)

#define INT_STS_D_P_DONE_MASK			BIT(12)
#define INT_STS_DONE_INT_MASK			BIT(2)
#define INT_STS_ERROR_FLAGS_MASK		0x00f4c860

#define MCTRL_INT_PCAP_LPBK_MASK		BIT(4)

/*
 * Xilinx 7-Series Bitstream Composition:
 *
 * Bitstream can be provided with an optinal header (`struct bs_header`).
 * The true bitstream starts with the binary-header composed of 13 words:
 *
 *  0: 0xFFFFFFFF (Dummy pad word)
 *     ...
 *  7: 0xFFFFFFFF (Dummy pad word)
 *  8: 0x000000BB (Bus width auto detect word 1)
 *  9: 0x11220044 (Bus width auto detect word 2)
 * 10: 0xFFFFFFFF (Dummy pad word)
 * 11: 0xFFFFFFFF (Dummy pad word)
 * 12: 0xAA995566 (Sync word)
 *
 * See Xilinx UG470 (v1.13.1) August 20 2018, Chapter 5 "Configuration
 * Details - Bitstream Composition" for further details.
 */

#define DUMMY_WORD				0xFFFFFFFF
#define BUS_WIDTH_AUTO_DETECT1_OFFSET		8
#define BUS_WIDTH_AUTO_DETECT1		0x000000BB
#define BUS_WIDTH_AUTO_DETECT2_OFFSET		9
#define BUS_WIDTH_AUTO_DETECT2		0x11220044
#define SYNC_WORD_OFFSET			12
#define SYNC_WORD				0xAA995566
#define BIN_HEADER_LENGTH			13

#define DEVC_UNLOCK_CODE			0x757bdf0d

struct fpgamgr {
	struct firmware_handler fh;
	struct device_d dev;
	void __iomem *regs;
	const struct zynq_devc_ops *devc_ops;
	int programmed;
	char *buf;
	size_t size;
	u32 features;
};

struct zynq_devc_ops {
	int (*fpga_load)(struct fpgamgr *mgr, u64 address, u32 size, u32 flags);
};

const struct zynq_devc_ops *zynq_get_devc_ops(void);

#endif /* FIRMWARE_ZYNQ_H_ */
