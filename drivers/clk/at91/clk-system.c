// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Copyright (C) 2013 Boris BREZILLON <b.brezillon@overkiz.com>
 */
#include <common.h>
#include <clock.h>
#include <io.h>
#include <linux/list.h>
#include <linux/clk.h>
#include <linux/clk/at91_pmc.h>
#include <mfd/syscon.h>
#include <linux/regmap.h>

#include "pmc.h"

#define SYSTEM_MAX_ID		31

#define SYSTEM_MAX_NAME_SZ	32

#define to_clk_system(_hw) container_of(_hw, struct clk_system, hw)
struct clk_system {
	struct clk_hw hw;
	struct regmap *regmap;
	u8 id;
	const char *parent_name;
};

static inline int is_pck(int id)
{
	return (id >= 8) && (id <= 15);
}

static inline bool clk_system_ready(struct regmap *regmap, int id)
{
	unsigned int status;

	regmap_read(regmap, AT91_PMC_SR, &status);

	return status & (1 << id) ? 1 : 0;
}

static int clk_system_enable(struct clk_hw *hw)
{
	struct clk_system *sys = to_clk_system(hw);

	regmap_write(sys->regmap, AT91_PMC_SCER, 1 << sys->id);

	if (!is_pck(sys->id))
		return 0;

	while (!clk_system_ready(sys->regmap, sys->id))
		barrier();

	return 0;
}

static void clk_system_disable(struct clk_hw *hw)
{
	struct clk_system *sys = to_clk_system(hw);

	regmap_write(sys->regmap, AT91_PMC_SCDR, 1 << sys->id);
}

static int clk_system_is_enabled(struct clk_hw *hw)
{
	struct clk_system *sys = to_clk_system(hw);
	unsigned int status;

	regmap_read(sys->regmap, AT91_PMC_SCSR, &status);

	if (!(status & (1 << sys->id)))
		return 0;

	if (!is_pck(sys->id))
		return 1;

	regmap_read(sys->regmap, AT91_PMC_SR, &status);

	return status & (1 << sys->id) ? 1 : 0;
}

static const struct clk_ops system_ops = {
	.enable = clk_system_enable,
	.disable = clk_system_disable,
	.is_enabled = clk_system_is_enabled,
};

struct clk * __init
at91_clk_register_system(struct regmap *regmap, const char *name,
			 const char *parent_name, u8 id)
{
	struct clk_system *sys;
	int ret;

	if (!parent_name || id > SYSTEM_MAX_ID)
		return ERR_PTR(-EINVAL);

	sys = xzalloc(sizeof(*sys));
	sys->hw.clk.name = name;
	sys->hw.clk.ops = &system_ops;
	sys->parent_name = parent_name;
	sys->hw.clk.parent_names = &sys->parent_name;
	sys->hw.clk.num_parents = 1;
	/* init.flags = CLK_SET_RATE_PARENT; */
	sys->id = id;
	sys->regmap = regmap;

	ret = bclk_register(&sys->hw.clk);
	if (ret) {
		kfree(sys);
		return ERR_PTR(ret);
	}

	return &sys->hw.clk;
}
