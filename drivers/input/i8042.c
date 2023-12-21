// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2002 ELTEC Elektronik AG
 * Frank Gottschling <fgottschling@eltec.de>
 */

/* i8042.c - Intel 8042 keyboard driver routines */

#include <common.h>
#include <init.h>
#include <clock.h>
#include <poller.h>
#include <string.h>
#include <input/input.h>

#define I8042_DATA_REG	0x60	/* keyboard i/o buffer */
#define I8042_STS_REG	0x64	/* keyboard status read */
#define I8042_CMD_REG	0x64	/* keyboard ctrl write */

/* Status register bit defines */
#define STATUS_OBF	(1 << 0)
#define STATUS_IBF	(1 << 1)

/* Configuration byte bit defines */
#define CFG_KIRQ_EN	(1 << 0)
#define CFG_MIRQ_EN	(1 << 1)
#define CFG_SET_BIST	(1 << 2)
#define CFG_KCLK_DIS	(1 << 4)
#define CFG_MCLK_DIS	(1 << 5)
#define CFG_AT_TRANS	(1 << 6)

/* i8042 commands */
#define CMD_RD_CONFIG	0x20	/* read configuration byte */
#define CMD_WR_CONFIG	0x60	/* write configuration byte */
#define CMD_SELF_TEST	0xaa	/* controller self-test */
#define CMD_KBD_DIS	0xad	/* keyboard disable */
#define CMD_KBD_EN	0xae	/* keyboard enable */
#define CMD_SET_KBD_LED	0xed	/* set keyboard led */
#define CMD_DRAIN_OUTPUT 0xf4   /* drain output buffer */
#define CMD_RESET_KBD	0xff	/* reset keyboard */

/* i8042 command result */
#define KBC_TEST_OK	0x55
#define KBD_ACK		0xfa
#define KBD_POR		0xaa

#define KBD_TIMEOUT	1000	/* 1 sec */
#define KBD_RESET_TRIES	3

#define NORMAL		0x0000	/* normal key */

struct i8042_kbd_pdata;

struct i8042_kbd_pdata {
	bool			extended;
	struct poller_async	poller;
	int			intinterval;
	struct input_device	input;
};

static unsigned char ext_key_map[] = {
	0x1c, /* keypad enter */
	0x1d, /* right control */
	0x35, /* keypad slash */
	0x37, /* print screen */
	0x38, /* right alt */
	0x46, /* break */
	0x47, /* editpad home */
	0x48, /* editpad up */
	0x49, /* editpad pgup */
	0x4b, /* editpad left */
	0x4d, /* editpad right */
	0x4f, /* editpad end */
	0x50, /* editpad dn */
	0x51, /* editpad pgdn */
	0x52, /* editpad ins */
	0x53, /* editpad del */
	0x00  /* map end */
	};

static void i8042_kbd_poll(void *arg)
{
	struct i8042_kbd_pdata *data = arg;
	int i, scan_code;
	bool release = false;

	if ((inb(I8042_STS_REG) & STATUS_OBF) == 0)
		goto out;

//	pr_info("DBG: %s\n", __func__);

	scan_code = inb(I8042_DATA_REG);
	if (scan_code == 0xfa) {
		goto out;
	} else if (scan_code == 0xe0) {
		data->extended = true;
		goto out;
	}

	if (scan_code & 0x80) {
		scan_code &= 0x7f;
		release = true;
	}

	if (data->extended) {
		data->extended = false;
		for (i = 0; ext_key_map[i]; i++) {
			if (ext_key_map[i] == scan_code) {
				scan_code = 0x60 + i;
				break;
			}
		}
		/* not found ? */
		if (!ext_key_map[i])
			goto out;
	}

//	pr_info("DBG: %s 0x%02x %d\n", __func__, scan_code, release);

	input_report_key_event(&data->input, scan_code, !release);

out:
	poller_call_async(&data->poller, data->intinterval * MSECOND, i8042_kbd_poll, data);
	return;
}

static int kbd_input_empty(void)
{
	int kbd_timeout = KBD_TIMEOUT * 1000;

	while ((inb(I8042_STS_REG) & STATUS_IBF) && kbd_timeout--)
		udelay(1);

	return kbd_timeout != -1;
}

static int kbd_output_full(void)
{
	int kbd_timeout = KBD_TIMEOUT * 1000;

	while (((inb(I8042_STS_REG) & STATUS_OBF) == 0) && kbd_timeout--)
		udelay(1);

	return kbd_timeout != -1;
}

static int i8042_kbd_update_leds(int leds)
{
	kbd_input_empty();
	outb(CMD_SET_KBD_LED, I8042_DATA_REG);
	kbd_input_empty();
	outb(leds & 0x7, I8042_DATA_REG);

	return 0;
}

static int kbd_write(int reg, int value)
{
	if (!kbd_input_empty())
		return -1;

	outb(value, reg);

	return 0;
}

static int kbd_read(int reg)
{
	if (!kbd_output_full())
		return -1;

	return inb(reg);
}

static int kbd_cmd_read(int cmd)
{
	if (kbd_write(I8042_CMD_REG, cmd))
		return -1;

	return kbd_read(I8042_DATA_REG);
}

static int kbd_cmd_write(int cmd, int data)
{
	if (kbd_write(I8042_CMD_REG, cmd))
		return -1;

	return kbd_write(I8042_DATA_REG, data);
}

static int kbd_reset(void)
{
	int config;

	/* controller self test */
	if (kbd_cmd_read(CMD_SELF_TEST) != KBC_TEST_OK)
		goto err;

	/* keyboard reset */
	if (kbd_write(I8042_DATA_REG, CMD_RESET_KBD) ||
	    kbd_read(I8042_DATA_REG) != KBD_ACK ||
	    kbd_read(I8042_DATA_REG) != KBD_POR)
		goto err;

	if (kbd_write(I8042_DATA_REG, CMD_DRAIN_OUTPUT) ||
	    kbd_read(I8042_DATA_REG) != KBD_ACK)
		goto err;

	/* set AT translation and disable irq */
	config = kbd_cmd_read(CMD_RD_CONFIG);
	if (config == -1)
		goto err;

	config |= CFG_AT_TRANS;
	config &= ~(CFG_KIRQ_EN | CFG_MIRQ_EN);
	if (kbd_cmd_write(CMD_WR_CONFIG, config))
		goto err;

	/* enable keyboard */
	if (kbd_write(I8042_CMD_REG, CMD_KBD_EN) ||
	    !kbd_input_empty())
		goto err;

	return 0;
err:
	pr_debug("%s: Keyboard failure\n", __func__);
	return -1;
}

static int kbd_controller_present(void)
{
	return inb(I8042_STS_REG) != 0xff;
}

static int __init i8042_kbd_init(void)
{
	struct i8042_kbd_pdata *data;
	int ret, try;

	pr_info("DBG: %s\n", __func__);

	if (!kbd_controller_present()) {
		pr_err("i8042 keyboard controller is not present\n");
		return -ENOENT;
	}

	for (try = 0; kbd_reset() != 0; try++) {
		if (try >= KBD_RESET_TRIES)
			return -1;
	}

	data = xzalloc(sizeof(struct i8042_kbd_pdata));
	data->intinterval = 100;

	ret = input_device_register(&data->input);
	if (ret) {
		pr_err("can't register input\n");
		return ret;
	}

	i8042_kbd_update_leds(NORMAL);
	pr_debug("%s: started\n", __func__);

	ret = poller_async_register(&data->poller, "i8042-kbd");
	if (ret) {
		pr_err("can't setup poller\n");
		return ret;
	}

	poller_call_async(&data->poller, data->intinterval * MSECOND, i8042_kbd_poll, data);

	return 0;
}
device_initcall(i8042_kbd_init);

