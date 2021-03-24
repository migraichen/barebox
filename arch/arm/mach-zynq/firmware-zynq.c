/* SPDX-License-Identifier: GPL-2.0 */
/*
 */
#include <common.h>
#include <init.h>
#include <linux/iopoll.h>
#include <mach/firmware-zynq.h>

/*
 * zynq_devc_fpga_load - Perform the fpga load
 * @mgr:	FPGA-Manager
 * @address:	Address to write to
 * @size:	PL bitstream size
 * @flags:	Flags - unused
 *
 * This function provides access to PCAP to transfer
 * the required bitstream into PL.
 *
 * Return:	Returns status, either success or error+reason
 */
static int zynq_devc_fpga_load(struct fpgamgr *mgr, u64 address,
				u32 size, u32 flags)
{
	unsigned long reg;

	if (!address || !size)
		return -EINVAL;

	/*
	 * The Programming Seqenze, see ug585 (v.12.2) Juny 1, 2018 Chapter
	 * 6.4.2 on page 211 Configure the PL via PCAP Bridge Example for
	 * detailed information to this Sequenze
	 */

	/* Enable the PCAP bridge and select PCAP for reconfiguration */
	reg = readl(mgr->regs + CTRL_OFFSET);
	reg |= ( CTRL_PCAP_PR_MASK | CTRL_PCAP_MODE_MASK );
	writel(reg, mgr->regs + CTRL_OFFSET);

	/* Clear the Interrupts */
	writel(0xffffffff, mgr->regs + INT_STS_OFFSET);

	/* Initialize the PL */
	reg = readl(mgr->regs + CTRL_OFFSET);
	reg |= CTRL_PCFG_PROG_B_MASK;
	writel(reg, mgr->regs + CTRL_OFFSET);

	reg = readl(mgr->regs + CTRL_OFFSET);
	reg &= ~CTRL_PCFG_PROG_B_MASK;
	writel(reg, mgr->regs + CTRL_OFFSET);

	readl_poll_timeout(mgr->regs + STATUS_OFFSET, reg,
			!(reg & STATUS_PCFG_INIT_MASK), 100 * USEC_PER_MSEC);

	reg = readl(mgr->regs + CTRL_OFFSET);
	reg |= CTRL_PCFG_PROG_B_MASK;
	writel(reg, mgr->regs + CTRL_OFFSET);

	/* Clear the Interrupts */
	writel(0xffffffff, mgr->regs + INT_STS_OFFSET);

	/* Ensure that the PL is ready for programming */
	readl_poll_timeout(mgr->regs + STATUS_OFFSET, reg,
			(reg & STATUS_PCFG_INIT_MASK), 100 * USEC_PER_MSEC);

	/* Check that there is room in the Command Queue */
	readl_poll_timeout(mgr->regs + STATUS_OFFSET, reg,
			!(reg & STATUS_DMA_CMD_Q_F_MASK), 100 * USEC_PER_MSEC);

	/* Disable the PCAP loopback */
	reg = readl(mgr->regs + MCTRL_OFFSET);
	reg &= ~MCTRL_INT_PCAP_LPBK_MASK;
	writel(reg, mgr->regs + MCTRL_OFFSET);

	/* Program the PCAP_2x clock divider */
	reg = readl(mgr->regs + CTRL_OFFSET);
	reg &= ~CTRL_PCAP_RATE_EN_MASK;
	writel(reg, mgr->regs + CTRL_OFFSET);

	/* Source Address: Location of bitstream */
	writel(address, mgr->regs + DMA_SRC_ADDR_OFFSET);

	/* Destination Address: 0xFFFF_FFFF */
	writel(0xffffffff, mgr->regs + DMA_DST_ADDR_OFFSET);

	/* Source Length: Total number of 32-bit words in the bitstream */
	writel((size >> 2), mgr->regs + DMA_SRC_LEN_OFFSET);

	/* Destination Length: Total number of 32-bit words in the bitstream */
	writel((size >> 2), mgr->regs + DMA_DEST_LEN_OFFSET);

	/* Wait for the DMA transfer to be done */
	readl_poll_timeout(mgr->regs + INT_STS_OFFSET, reg,
			(reg & INT_STS_D_P_DONE_MASK), 100 * USEC_PER_MSEC);

	/* Check for errors */
	if(reg & INT_STS_ERROR_FLAGS_MASK) {
		printf("interrupt status register (0x%04lx)\n", reg);
		return -EIO;
	}

	/* Wait for the DMA transfer to be done */
	readl_poll_timeout(mgr->regs + INT_STS_OFFSET, reg,
			(reg & INT_STS_DONE_INT_MASK), 100 * USEC_PER_MSEC);

	printf("FPGA config done\n");

	return 0;
}

static const struct zynq_devc_ops devc_ops = {
	.fpga_load = zynq_devc_fpga_load,
};

/**
 * zynq_get_devc_ops - Get devc ops functions
 *
 * Return: Pointer of devc_ops structure
 */
const struct zynq_devc_ops *zynq_get_devc_ops(void)
{
	return &devc_ops;
}
EXPORT_SYMBOL_GPL(zynq_get_devc_ops);
