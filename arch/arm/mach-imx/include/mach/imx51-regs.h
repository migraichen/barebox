#ifndef __MACH_IMX51_REGS_H
#define __MACH_IMX51_REGS_H

/* WEIM registers */
#define WEIM_CSxGCR1(n)	(((n) * 0x18) + 0x00)
#define WEIM_CSxGCR2(n)	(((n) * 0x18) + 0x04)
#define WEIM_CSxRCR1(n)	(((n) * 0x18) + 0x08)
#define WEIM_CSxRCR2(n)	(((n) * 0x18) + 0x0c)
#define WEIM_CSxWCR1(n)	(((n) * 0x18) + 0x10)
#define WEIM_WCR	0x90
#define WEIM_WIAR	0x94
#define WEIM_EAR	0x98

#define MX51_IROM_BASE_ADDR	0x0

#define MX51_IPU_BASE_ADDR	0x40000000

/*
 * AIPS 1
 */
#define MX51_AIPS1_BASE_ADDR 	0x73F00000

#define MX51_OTG_BASE_ADDR	(MX51_AIPS1_BASE_ADDR + 0x00080000)
#define MX51_GPIO1_BASE_ADDR	(MX51_AIPS1_BASE_ADDR + 0x00084000)
#define MX51_GPIO2_BASE_ADDR	(MX51_AIPS1_BASE_ADDR + 0x00088000)
#define MX51_GPIO3_BASE_ADDR	(MX51_AIPS1_BASE_ADDR + 0x0008C000)
#define MX51_GPIO4_BASE_ADDR	(MX51_AIPS1_BASE_ADDR + 0x00090000)
#define MX51_KPP_BASE_ADDR	(MX51_AIPS1_BASE_ADDR + 0x00094000)
#define MX51_WDOG_BASE_ADDR	(MX51_AIPS1_BASE_ADDR + 0x00098000)
#define MX51_WDOG2_BASE_ADDR	(MX51_AIPS1_BASE_ADDR + 0x0009C000)
#define MX51_GPT1_BASE_ADDR	(MX51_AIPS1_BASE_ADDR + 0x000A0000)
#define MX51_SRTC_BASE_ADDR	(MX51_AIPS1_BASE_ADDR + 0x000A4000)
#define MX51_IOMUXC_BASE_ADDR	(MX51_AIPS1_BASE_ADDR + 0x000A8000)
#define MX51_EPIT1_BASE_ADDR	(MX51_AIPS1_BASE_ADDR + 0x000AC000)
#define MX51_EPIT2_BASE_ADDR	(MX51_AIPS1_BASE_ADDR + 0x000B0000)
#define MX51_PWM1_BASE_ADDR	(MX51_AIPS1_BASE_ADDR + 0x000B4000)
#define MX51_PWM2_BASE_ADDR	(MX51_AIPS1_BASE_ADDR + 0x000B8000)
#define MX51_UART1_BASE_ADDR	(MX51_AIPS1_BASE_ADDR + 0x000BC000)
#define MX51_UART2_BASE_ADDR	(MX51_AIPS1_BASE_ADDR + 0x000C0000)
#define MX51_SRC_BASE_ADDR	(MX51_AIPS1_BASE_ADDR + 0x000D0000)
#define MX51_CCM_BASE_ADDR	(MX51_AIPS1_BASE_ADDR + 0x000D4000)
#define MX51_GPC_BASE_ADDR	(MX51_AIPS1_BASE_ADDR + 0x000D8000)

/*
 * AIPS 2
 */
#define MX51_AIPS2_BASE_ADDR		0x83F00000

#define MX51_PLL1_BASE_ADDR	(MX51_AIPS2_BASE_ADDR + 0x00080000)
#define MX51_PLL2_BASE_ADDR	(MX51_AIPS2_BASE_ADDR + 0x00084000)
#define MX51_PLL3_BASE_ADDR	(MX51_AIPS2_BASE_ADDR + 0x00088000)
#define MX51_AHBMAX_BASE_ADDR	(MX51_AIPS2_BASE_ADDR + 0x00094000)
#define MX51_IIM_BASE_ADDR	(MX51_AIPS2_BASE_ADDR + 0x00098000)
#define MX51_CSU_BASE_ADDR	(MX51_AIPS2_BASE_ADDR + 0x0009C000)
#define MX51_ARM_BASE_ADDR	(MX51_AIPS2_BASE_ADDR + 0x000A0000)
#define MX51_OWIRE_BASE_ADDR (MX51_AIPS2_BASE_ADDR + 0x000A4000)
#define MX51_FIRI_BASE_ADDR	(MX51_AIPS2_BASE_ADDR + 0x000A8000)
#define MX51_ECSPI2_BASE_ADDR	(MX51_AIPS2_BASE_ADDR + 0x000AC000)
#define MX51_SDMA_BASE_ADDR	(MX51_AIPS2_BASE_ADDR + 0x000B0000)
#define MX51_SCC_BASE_ADDR	(MX51_AIPS2_BASE_ADDR + 0x000B4000)
#define MX51_ROMCP_BASE_ADDR	(MX51_AIPS2_BASE_ADDR + 0x000B8000)
#define MX51_RTIC_BASE_ADDR	(MX51_AIPS2_BASE_ADDR + 0x000BC000)
#define MX51_CSPI_BASE_ADDR	(MX51_AIPS2_BASE_ADDR + 0x000C0000)
#define MX51_I2C2_BASE_ADDR	(MX51_AIPS2_BASE_ADDR + 0x000C4000)
#define MX51_I2C1_BASE_ADDR	(MX51_AIPS2_BASE_ADDR + 0x000C8000)
#define MX51_SSI1_BASE_ADDR	(MX51_AIPS2_BASE_ADDR + 0x000CC000)
#define MX51_AUDMUX_BASE_ADDR	(MX51_AIPS2_BASE_ADDR + 0x000D0000)
#define MX51_M4IF_BASE_ADDR	(MX51_AIPS2_BASE_ADDR + 0x000D8000)
#define MX51_ESDCTL_BASE_ADDR	(MX51_AIPS2_BASE_ADDR + 0x000D9000)
#define MX51_WEIM_BASE_ADDR	(MX51_AIPS2_BASE_ADDR + 0x000DA000)
#define MX51_NFC_BASE_ADDR	(MX51_AIPS2_BASE_ADDR + 0x000DB000)
#define MX51_EMI_BASE_ADDR	(MX51_AIPS2_BASE_ADDR + 0x000DBF00)
#define MX51_MIPI_HSC_BASE_ADDR	(MX51_AIPS2_BASE_ADDR + 0x000DC000)
#define MX51_ATA_BASE_ADDR	(MX51_AIPS2_BASE_ADDR + 0x000E0000)
#define MX51_SIM_BASE_ADDR	(MX51_AIPS2_BASE_ADDR + 0x000E4000)
#define MX51_SSI3_BASE_ADDR	(MX51_AIPS2_BASE_ADDR + 0x000E8000)
#define MX51_MXC_FEC_BASE_ADDR	(MX51_AIPS2_BASE_ADDR + 0x000EC000)
#define MX51_TVE_BASE_ADDR	(MX51_AIPS2_BASE_ADDR + 0x000F0000)
#define MX51_VPU_BASE_ADDR	(MX51_AIPS2_BASE_ADDR + 0x000F4000)
#define MX51_SAHARA_BASE_ADDR	(MX51_AIPS2_BASE_ADDR + 0x000F8000)

#define MX51_SPBA0_BASE_ADDR    0x70000000
#define MX51_MMC_SDHC1_BASE_ADDR	(MX51_SPBA0_BASE_ADDR + 0x00004000)
#define MX51_MMC_SDHC2_BASE_ADDR	(MX51_SPBA0_BASE_ADDR + 0x00008000)
#define MX51_UART3_BASE_ADDR 		(MX51_SPBA0_BASE_ADDR + 0x0000C000)
#define MX51_ECSPI1_BASE_ADDR 		(MX51_SPBA0_BASE_ADDR + 0x00010000)
#define MX51_SSI2_BASE_ADDR		(MX51_SPBA0_BASE_ADDR + 0x00014000)
#define MX51_MMC_SDHC3_BASE_ADDR	(MX51_SPBA0_BASE_ADDR + 0x00020000)
#define MX51_MMC_SDHC4_BASE_ADDR	(MX51_SPBA0_BASE_ADDR + 0x00024000)
#define MX51_SPDIF_BASE_ADDR		(MX51_SPBA0_BASE_ADDR + 0x00028000)
#define MX51_ATA_DMA_BASE_ADDR		(MX51_SPBA0_BASE_ADDR + 0x00030000)
#define MX51_SLIM_DMA_BASE_ADDR		(MX51_SPBA0_BASE_ADDR + 0x00034000)
#define MX51_HSI2C_DMA_BASE_ADDR	(MX51_SPBA0_BASE_ADDR + 0x00038000)
#define MX51_SPBA_CTRL_BASE_ADDR	(MX51_SPBA0_BASE_ADDR + 0x0003C000)

#define MX51_NFC_AXI_BASE_ADDR	0xcfff0000

/*
 * Memory regions and CS
 */
#define MX51_GPU_CTRL_BASE_ADDR	0x30000000
#define MX51_IPU_CTRL_BASE_ADDR	0x40000000
#define MX51_CSD0_BASE_ADDR		0x90000000
#define MX51_CSD1_BASE_ADDR		0xA0000000
#define MX51_CS0_BASE_ADDR		0xB0000000
#define MX51_CS1_BASE_ADDR		0xB8000000
#define MX51_CS2_BASE_ADDR		0xC0000000
#define MX51_CS3_BASE_ADDR		0xC8000000
#define MX51_CS4_BASE_ADDR		0xCC000000
#define MX51_CS5_BASE_ADDR		0xCE000000

#endif /* __MACH_IMX51_REGS_H */