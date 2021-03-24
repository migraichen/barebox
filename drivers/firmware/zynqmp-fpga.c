// SPDX-License-Identifier: GPL-2.0
/*
 * Xilinx Zynq MPSoC PL loading
 *
 * Copyright (c) 2018 Thomas Haemmerle <thomas.haemmerle@wolfvision.net>
 *
 * based on U-Boot zynqmppl code
 *
 * (C) Copyright 2015 - 2016, Xilinx, Inc,
 * Michal Simek <michal.simek@xilinx.com>
 * Siva Durga Prasad <siva.durga.paladugu@xilinx.com> *
 */

#include <firmware.h>
#include <common.h>
#include <init.h>
#include <dma.h>
#ifdef CONFIG_FIRMWARE_ZYNQMP_FPGA
#include <mach/firmware-zynqmp.h>
#endif
#ifdef CONFIG_FIRMWARE_ZYNQ7000_FPGA
#include <mach/firmware-zynq.h>
#include <mach/zynq7000-regs.h>
#endif

enum xilinx_byte_order {
	XILINX_BYTE_ORDER_BIT,
	XILINX_BYTE_ORDER_BIN,
};

struct bs_header {
	__be16 length;
	u8 padding[9];
	__be16 size;
	char entries[0];
} __attribute__ ((packed));

struct bs_header_entry {
	char type;
	__be16 length;
	char data[0];
} __attribute__ ((packed));

static void copy_words_swapped(u32 *dst, const u32 *src, size_t size)
{
	int i;

	for (i = 0; i < size; i++)
		dst[i] = __swab32(src[i]);
}

static int get_byte_order(const u32 *bin_header, size_t size,
			  enum xilinx_byte_order *byte_order)
{
	if (size < BIN_HEADER_LENGTH * sizeof(*bin_header))
		return -EINVAL;

	if (bin_header[SYNC_WORD_OFFSET] == SYNC_WORD) {
		*byte_order = XILINX_BYTE_ORDER_BIT;
		return 0;
	}

	if (bin_header[SYNC_WORD_OFFSET] == __swab32(SYNC_WORD)) {
		*byte_order = XILINX_BYTE_ORDER_BIN;
		return 0;
	}

	return -EINVAL;
}

static bool is_bin_header_valid(const u32 *bin_header, size_t size,
				enum xilinx_byte_order byte_order)
{
	size_t i;

	if (size < BIN_HEADER_LENGTH * sizeof(*bin_header))
		return false;

	for (i = 0; i < BIN_HEADER_LENGTH; i++, bin_header++) {
		u32 current;
		u32 expected;

		if (byte_order == XILINX_BYTE_ORDER_BIT)
			current = *bin_header;
		else
			current = __swab32(*bin_header);

		switch (i) {
		case BUS_WIDTH_AUTO_DETECT1_OFFSET:
			expected = BUS_WIDTH_AUTO_DETECT1;
			break;
		case BUS_WIDTH_AUTO_DETECT2_OFFSET:
			expected = BUS_WIDTH_AUTO_DETECT2;
			break;
		case SYNC_WORD_OFFSET:
			expected = SYNC_WORD;
			break;
		default:
			expected = DUMMY_WORD;
			break;
		}

		if (current != expected)
			return false;
	}

	return true;
}

static int get_header_length(const char *header, size_t size)
{
	u32 *buf_u32;
	int p;

	for (p = 0; p < size; p++) {
		buf_u32 = (u32 *)&header[p];
		if (*buf_u32 == DUMMY_WORD)
			return p;
	}
	return -EINVAL;
}

static void zynqmp_fpga_show_header(const struct device_d *dev,
			 struct bs_header *header, size_t size)
{
	struct bs_header_entry *entry;
	unsigned int i;
	unsigned int length;

	for (i = 0; i < size - sizeof(*header); i += sizeof(*entry) + length) {
		entry = (struct bs_header_entry *)&header->entries[i];
		length = __be16_to_cpu(entry->length);

		switch (entry->type) {
		case 'a':
			printf("Design: %s\n", entry->data);
			break;
		case 'b':
			printf("Part number: %s\n", entry->data);
			break;
		case 'c':
			printf("Date: %s\n", entry->data);
			break;
		case 'd':
			printf("Time: %s\n", entry->data);
			break;
		case 'e':
			/* Size entry does not have a length but is be32 int */
			printf("Size: %d bytes\n",
			       (length << 16) + (entry->data[0] << 8) + entry->data[1]);
			return;
		default:
			dev_warn(dev, "Invalid header entry: %c", entry->type);
			return;
		}
	}
}

static int fpgamgr_program_finish(struct firmware_handler *fh)
{
	struct fpgamgr *mgr = container_of(fh, struct fpgamgr, fh);
	char *buf_aligned;
	u32 *buf_size = NULL;
	u32 *body;
	size_t body_length;
	int header_length = 0;
	enum xilinx_byte_order byte_order;
	u64 addr;
	int status = 0;
	u8 flags = 0;

	if (!mgr->buf) {
		status = -ENOBUFS;
		dev_err(&mgr->dev, "buffer is NULL\n");
		goto err_free;
	}

	header_length = get_header_length(mgr->buf, mgr->size);
	if (header_length < 0) {
		status = header_length;
		goto err_free;
	}
	zynqmp_fpga_show_header(&mgr->dev,
			      (struct bs_header *)mgr->buf, header_length);

	body = (u32 *)&mgr->buf[header_length];
	body_length = mgr->size - header_length;

	status = get_byte_order(body, body_length, &byte_order);
	if (status < 0)
		goto err_free;

	if (!is_bin_header_valid(body, body_length, byte_order)) {
		dev_err(&mgr->dev, "Invalid bitstream header\n");
		status =  -EINVAL;
		goto err_free;
	}

	buf_aligned = dma_alloc_coherent(body_length, DMA_ADDRESS_BROKEN);
	if (!buf_aligned) {
		status = -ENOBUFS;
		goto err_free;
	}

#ifdef CONFIG_FIRMWARE_ZYNQMP_FPGA
	if (!(mgr->features & ZYNQMP_PM_FEATURE_SIZE_NOT_NEEDED)) {
		buf_size = dma_alloc_coherent(sizeof(*buf_size),
		DMA_ADDRESS_BROKEN);
		if (!buf_size) {
			status = -ENOBUFS;
			goto err_free;
		}
		*buf_size = body_length;
	}

	if (!(mgr->features & ZYNQMP_PM_FEATURE_BYTE_ORDER_IRREL) &&
	    byte_order == XILINX_BYTE_ORDER_BIN)
		copy_words_swapped((u32 *)buf_aligned, body,
				   body_length / sizeof(u32));
	else
		memcpy((u32 *)buf_aligned, body, body_length);

	addr = (u64)buf_aligned;

	/* we do not provide a header */
	flags |= ZYNQMP_FPGA_BIT_ONLY_BIN;

	if (!(mgr->features & ZYNQMP_PM_FEATURE_SIZE_NOT_NEEDED) && buf_size) {
		status = mgr->eemi_ops->fpga_load(addr,
				(u32)(uintptr_t)buf_size,
				flags);
		dma_free_coherent(buf_size, 0, sizeof(*buf_size));
	} else {
		status = mgr->eemi_ops->fpga_load(addr, (u32)(body_length),
						  flags);
	}
#endif
#ifdef CONFIG_FIRMWARE_ZYNQ7000_FPGA
	/* UG585 (v1.12.2) July 1, 2018 Chapter 6.4.3
	 * In all modes, the DMA transactions must be 64-byte aligned to prevent
	 * accidently crossing a 4K byte boundary.
	 */
	if(byte_order == XILINX_BYTE_ORDER_BIN)
		copy_words_swapped((u32 *)buf_aligned, body, body_length / sizeof(u32));
	else
		memcpy((u32 *)buf_aligned, body, body_length);

	addr = (u32)buf_aligned;

	writel(0x0000DF0D, ZYNQ_SLCR_UNLOCK);
	writel(0x0000000f, ZYNQ_SLCR_BASE + 0x240); // assert FPGA resets

	writel(0x00000000, ZYNQ_SLCR_BASE + 0x900); // disable levelshifter
	writel(0x0000000a, ZYNQ_SLCR_BASE + 0x900); // enable levelshifter PS-PL

	status = mgr->devc_ops->fpga_load(mgr, addr, (u32)(body_length), 0);

	writel(0x0000000f, ZYNQ_SLCR_BASE + 0x900); // enable all levelshifter
	writel(0x00000000, ZYNQ_SLCR_BASE + 0x240); // deassert FPGA resets

	writel(0x0000767B, ZYNQ_SLCR_LOCK);
#endif
	if (status < 0)
		dev_err(&mgr->dev, "unable to load fpga\n");

	dma_free_coherent(buf_aligned, 0, body_length);

 err_free:
	free(mgr->buf);

	return status;
}

static int fpgamgr_program_write_buf(struct firmware_handler *fh,
		const void *buf, size_t size)
{
	struct fpgamgr *mgr = container_of(fh, struct fpgamgr, fh);

	/* Since write() is called by copy_file, we only receive chuncks with
	 * size RW_BUF_SIZE of the bitstream.
	 * Buffer the chunks here and handle it in close()
	 */

	mgr->buf = realloc(mgr->buf, mgr->size + size);
	if (!mgr->buf)
		return -ENOBUFS;

	memcpy(&(mgr->buf[mgr->size]), buf, size);
	mgr->size += size;

	return 0;
}

static int fpgamgr_program_start(struct firmware_handler *fh)
{
	struct fpgamgr *mgr = container_of(fh, struct fpgamgr, fh);

	mgr->size = 0;
	mgr->buf = NULL;

	return 0;
}


static int programmed_get(struct param_d *p, void *priv)
{
#ifdef CONFIG_FIRMWARE_ZYNQMP_FPGA
	struct fpgamgr *mgr = priv;
	u32 status = 0x00;
	int ret = 0;

	ret = mgr->eemi_ops->fpga_getstatus(&status);
	if (ret)
		return ret;

	mgr->programmed = !!(status & ZYNQMP_PCAP_STATUS_FPGA_DONE);
#endif
	return 0;
}

static int zynqmp_fpga_probe(struct device_d *dev)
{
	struct fpgamgr *mgr;
	struct firmware_handler *fh;
	const char *alias = of_alias_get(dev->device_node);
	const char *model = NULL;
	struct param_d *p;
	u32 api_version;
	struct resource *iores;
	int ret;

	mgr = xzalloc(sizeof(*mgr));
	fh = &mgr->fh;

	if (alias)
		fh->id = xstrdup(alias);
	else
		fh->id = xstrdup("zynqmp-fpga-manager");

	fh->open = fpgamgr_program_start;
	fh->write = fpgamgr_program_write_buf;
	fh->close = fpgamgr_program_finish;
	of_property_read_string(dev->device_node, "compatible", &model);
	if (model)
		fh->model = xstrdup(model);
	fh->dev = dev;

#ifdef CONFIG_FIRMWARE_ZYNQMP_FPGA
	mgr->eemi_ops = zynqmp_pm_get_eemi_ops();

	ret = mgr->eemi_ops->get_api_version(&api_version);
	if (ret) {
		dev_err(&mgr->dev, "could not get API version\n");
		goto out;
	}
	if (api_version >= ZYNQMP_PM_VERSION(1, 1))
		mgr->features |= ZYNQMP_PM_VERSION_1_1_FEATURES;
#endif
#ifdef CONFIG_FIRMWARE_ZYNQ7000_FPGA
	iores = dev_request_mem_resource(dev, 0);
	if (IS_ERR(iores)) {
		ret = PTR_ERR(iores);
		goto out;
	}
	mgr->regs = IOMEM(iores->start);
	mgr->devc_ops = zynq_get_devc_ops();
	/* Unlock DevC in case BootROM did not do it */
	writel(DEVC_UNLOCK_CODE, mgr->regs + UNLOCK_OFFSET);
#endif
	mgr->features = 0;

	dev_dbg(dev, "Registering ZynqMP FPGA programmer\n");
	mgr->dev.id = DEVICE_ID_SINGLE;
	dev_set_name(&mgr->dev, "zynqmp_fpga");
	mgr->dev.parent = dev;
	ret = register_device(&mgr->dev);
	if (ret)
		goto out;

	p = dev_add_param_bool(&mgr->dev, "programmed", NULL, programmed_get,
			&mgr->programmed, mgr);
	if (IS_ERR(p)) {
		ret = PTR_ERR(p);
		goto out_unreg;
	}

	fh->dev = &mgr->dev;
	ret = firmwaremgr_register(fh);
	if (ret != 0) {
		free(mgr);
		goto out_unreg;
	}

	return 0;
out_unreg:
	unregister_device(&mgr->dev);
out:
	free(fh->id);
	free(mgr);

	return ret;
}

static struct of_device_id zynqmpp_fpga_id_table[] = {
	{ .compatible = "xlnx,zynqmp-pcap-fpga" },
	{ .compatible = "xlnx,zynq-devcfg-1.0" },
	{ /* sentinel */ }
};

static struct driver_d zynqmp_fpga_driver = {
	.name = "zynqmp_fpga_manager",
	.of_compatible = DRV_OF_COMPAT(zynqmpp_fpga_id_table),
	.probe = zynqmp_fpga_probe,
};
device_platform_driver(zynqmp_fpga_driver);
