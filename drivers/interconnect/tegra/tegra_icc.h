/*
 * Copyright (c) 2020 NVIDIA Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _TEGRA_INTERCONNECT_H_
#define _TEGRA_INTERCONNECT_H_

#include <linux/clk.h>
#include <linux/device.h>
#include <linux/interconnect-provider.h>

struct tegra_icc_provider {
	struct icc_provider provider;
	struct device *dev;
	struct clk *dram_clk;
	uint64_t rate;
	uint64_t max_dram_rate;
};

#define to_tegra_provider(_provider) \
	container_of(_provider, struct tegra_icc_provider, provider)

enum tegra_icc_client_type {
	TEGRA_ICC_NONE,
	TEGRA_ICC_NISO,
	TEGRA_ICC_ISO_DISPLAY,
	TEGRA_ICC_ISO_VI,
	TEGRA_ICC_ISO_OTHER,
};

/*
 * struct tegra_icc_node - Tegra specific interconnect nodes
 * @name: the node name used in debugfs
 * @id: a unique node identifier
 * @type: iso or niso
 */
struct tegra_icc_node {
	const char *name;
	u16 id;
	enum tegra_icc_client_type type;
};

struct tegra_icc_ops {
	int (*plat_icc_set)(struct icc_node *, struct icc_node *);
	int (*plat_icc_aggregate)(struct icc_node *, u32, u32, u32 *, u32 *);
};

#define DEFINE_TNODE(_name, _id, _type)				\
		static struct tegra_icc_node _name = {		\
		.id = _id,					\
		.name = #_name,					\
		.type = _type,					\
	}

extern const struct tegra_icc_ops tegra23x_icc_ops;

#endif /* _TEGRA_INTERCONNECT_H_ */
