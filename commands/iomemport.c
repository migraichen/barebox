// SPDX-License-Identifier: GPL-2.0-only
// SPDX-FileCopyrightText: © 2011 Sascha Hauer <s.hauer@pengutronix.de>, Pengutronix

/* iomem.c - barebox iomem command */

#include <asm/io.h>
#include <common.h>
#include <command.h>

static void __print_resources(struct resource *res, int indent)
{
	struct resource *r;
	resource_size_t size = resource_size(res);
	int i;

	for (i = 0; i < indent; i++)
		printf("  ");

	printf("%pa - %pa (size %pa) %s\n",
			&res->start, &res->end, &size, res->name);

	list_for_each_entry(r, &res->children, sibling)
		__print_resources(r, indent + 1);
}

static void print_resources(struct resource *res)
{
	__print_resources(res, 0);
}

static int do_iomem(int argc, char *argv[])
{
	print_resources(&iomem_resource);

	return 0;
}

static int do_outb(int argc, char *argv[])
{
	u16 port;
	u8 value;
	int ret;

	if (argc != 3)
		return COMMAND_ERROR_USAGE;

	ret = kstrtou16(argv[1], 16, &port);
	if (ret < 0)
		return ret;

	ret = kstrtou8(argv[2], 16, &value);
	if (ret < 0)
		return ret;

	__asm__ __volatile__ ("outb %b0, %w1" : : "a" (value), "Nd" (port));

	return 0;
}

static int do_outw(int argc, char *argv[])
{
	u16 port, value;
	int ret;

	if (argc != 3)
		return COMMAND_ERROR_USAGE;

	ret = kstrtou16(argv[1], 16, &port);
	if (ret < 0)
		return ret;

	ret = kstrtou16(argv[2], 16, &value);
	if (ret < 0)
		return ret;

	__asm__ __volatile__ ("outw %w0, %w1" : : "a" (value), "Nd" (port));

	return 0;
}

static int do_outl(int argc, char *argv[])
{
	u16 port;
	u32 value;
	int ret;

	if (argc != 3)
		return COMMAND_ERROR_USAGE;

	ret = kstrtou16(argv[1], 16, &port);
	if (ret < 0)
		return ret;

	ret = kstrtou32(argv[2], 16, &value);
	if (ret < 0)
		return ret;

	__asm__ __volatile__ ("outl %0, %w1" : : "a" (value), "Nd" (port));

	return 0;
}

static int do_inb(int argc, char *argv[])
{
	u16 port;
	u8 value;
	int ret;

	if (argc != 2)
		return COMMAND_ERROR_USAGE;

	ret = kstrtou16(argv[1], 16, &port);
	if (ret < 0)
		return ret;

	__asm__ __volatile__ ("inb %w1, %b0" : "=a"(value) : "Nd" (port));

	printf("0x%02x\n", value);

	return 0;
}

static int do_inw(int argc, char *argv[])
{
	u16 port, value;
	int ret;

	if (argc != 2)
		return COMMAND_ERROR_USAGE;

	ret = kstrtou16(argv[1], 16, &port);
	if (ret < 0)
		return ret;

	__asm__ __volatile__ ("inw %w1, %w0" : "=a"(value) : "Nd" (port));

	printf("0x%04x\n", value);

	return 0;
}

static int do_inl(int argc, char *argv[])
{
	u16 port;
	u32 value;
	int ret;

	if (argc != 2)
		return COMMAND_ERROR_USAGE;

	ret = kstrtou16(argv[1], 16, &port);
	if (ret < 0)
		return ret;

	__asm__ __volatile__ ("inl %w1, %0" : "=a"(value) : "Nd" (port));

	printf("0x%08x\n", value);

	return 0;
}

BAREBOX_CMD_START(iomem)
	.cmd		= do_iomem,
	BAREBOX_CMD_DESC("show IO memory usage")
	BAREBOX_CMD_GROUP(CMD_GRP_INFO)
BAREBOX_CMD_END

BAREBOX_CMD_START(outb)
	.cmd		= do_outb,
	BAREBOX_CMD_DESC("set IO memory port")
	BAREBOX_CMD_GROUP(CMD_GRP_INFO)
BAREBOX_CMD_END

BAREBOX_CMD_START(outw)
	.cmd		= do_outw,
	BAREBOX_CMD_DESC("set IO memory port")
	BAREBOX_CMD_GROUP(CMD_GRP_INFO)
BAREBOX_CMD_END

BAREBOX_CMD_START(outl)
	.cmd		= do_outl,
	BAREBOX_CMD_DESC("set IO memory port")
	BAREBOX_CMD_GROUP(CMD_GRP_INFO)
BAREBOX_CMD_END

BAREBOX_CMD_START(inb)
	.cmd		= do_inb,
	BAREBOX_CMD_DESC("read IO memory port")
	BAREBOX_CMD_GROUP(CMD_GRP_INFO)
BAREBOX_CMD_END

BAREBOX_CMD_START(inw)
	.cmd		= do_inw,
	BAREBOX_CMD_DESC("read IO memory port")
	BAREBOX_CMD_GROUP(CMD_GRP_INFO)
BAREBOX_CMD_END

BAREBOX_CMD_START(inl)
	.cmd		= do_inl,
	BAREBOX_CMD_DESC("read IO memory port")
	BAREBOX_CMD_GROUP(CMD_GRP_INFO)
BAREBOX_CMD_END

#if IO_SPACE_LIMIT > 0
static int do_ioport(int argc, char *argv[])
{
	print_resources(&ioport_resource);

	return 0;
}

BAREBOX_CMD_START(ioport)
	.cmd		= do_ioport,
	BAREBOX_CMD_DESC("show IO port usage")
	BAREBOX_CMD_GROUP(CMD_GRP_INFO)
BAREBOX_CMD_END
#endif
