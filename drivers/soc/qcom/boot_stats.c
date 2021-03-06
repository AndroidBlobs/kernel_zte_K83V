/* Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/cpu.h>
#include <linux/sched.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <soc/qcom/socinfo.h>

struct boot_stats {
	uint32_t bootloader_start;
	uint32_t bootloader_end;
	uint32_t bootloader_display;
	uint32_t bootloader_load_kernel;
};

static void __iomem *mpm_counter_base;
static uint32_t mpm_counter_freq;
static struct boot_stats __iomem *boot_stats;

static int mpm_parse_dt(void)
{
	struct device_node *np;
	u32 freq;

	np = of_find_compatible_node(NULL, NULL, "qcom,msm-imem-boot_stats");
	if (!np) {
		pr_err("can't find qcom,msm-imem node\n");
		return -ENODEV;
	}
	boot_stats = of_iomap(np, 0);
	if (!boot_stats) {
		pr_err("boot_stats: Can't map imem\n");
		return -ENODEV;
	}

	np = of_find_compatible_node(NULL, NULL, "qcom,mpm2-sleep-counter");
	if (!np) {
		pr_err("mpm_counter: can't find DT node\n");
		return -ENODEV;
	}

	if (!of_property_read_u32(np, "clock-frequency", &freq))
		mpm_counter_freq = freq;
	else
		return -ENODEV;

	if (of_get_address(np, 0, NULL, NULL)) {
		mpm_counter_base = of_iomap(np, 0);
		if (!mpm_counter_base) {
			pr_err("mpm_counter: cant map counter base\n");
			return -ENODEV;
		}
	}

	return 0;
}

static void print_boot_stats(void)
{
	pr_info("KPI: Bootloader start count = %u\n",
		readl_relaxed(&boot_stats->bootloader_start));
	pr_info("KPI: Bootloader end count = %u\n",
		readl_relaxed(&boot_stats->bootloader_end));
	pr_info("KPI: Bootloader display count = %u\n",
		readl_relaxed(&boot_stats->bootloader_display));
	pr_info("KPI: Bootloader load kernel count = %u\n",
		readl_relaxed(&boot_stats->bootloader_load_kernel));
	pr_info("KPI: Kernel MPM timestamp = %u\n",
		readl_relaxed(mpm_counter_base));
	pr_info("KPI: Kernel MPM Clock frequency = %u\n",
		mpm_counter_freq);
}

/*ZTE ADD for BOOT_MODE start*/
static int __init bootmode_init(char *mode)
{
	int boot_mode = 0;

	if (!strncmp(mode, ANDROID_BOOT_MODE_FTM, strlen(ANDROID_BOOT_MODE_FTM))) {
		boot_mode = ENUM_BOOT_MODE_FTM;
		pr_info("KERENEL:boot_mode:FTM\n");
	} else if (!strncmp(mode, ANDROID_BOOT_MODE_FFBM, strlen(ANDROID_BOOT_MODE_FFBM))) {
		boot_mode = ENUM_BOOT_MODE_FFBM;
		pr_info("KERENEL:boot_mode:FFBM\n");
	} else if (!strncmp(mode, ANDROID_BOOT_MODE_RECOVERY, strlen(ANDROID_BOOT_MODE_RECOVERY))) {
		boot_mode = ENUM_BOOT_MODE_RECOVERY;
		pr_info("KERENEL:boot_mode:RECOVERY\n");
	} else if (!strncmp(mode, ANDROID_BOOT_MODE_CHARGER, strlen(ANDROID_BOOT_MODE_CHARGER))) {
		boot_mode = ENUM_BOOT_MODE_CHARGER;
		pr_info("KERENEL:boot_mode:CHARGER\n");
	} else {
		boot_mode = ENUM_BOOT_MODE_NORMAL;
		pr_info("KERENEL:boot_mode:DEFAULT NORMAL\n");
	}

	socinfo_set_boot_mode(boot_mode);

	return 0;
}
__setup(ANDROID_BOOT_MODE, bootmode_init);

static int __init hardinfo_id_setup(char *str)
{
	int ret = 0;
	unsigned long hardware_id = 0;

	pr_info("%s @str:%s\n", __func__, str);

	ret = kstrtoul(str, 10, &hardware_id);
	if (ret != 0) {
		pr_info("%s, kstrtoul error\n", __func__);
	}
	pr_info("hardware_id =%ld\n", hardware_id);

	zte_set_hardinfo_id(hardware_id);

	return 0;
}
__setup("androidboot.hardinfo=", hardinfo_id_setup);

/*ZTE ADD for BOOT_MODE end*/

int boot_stats_init(void)
{
	int ret;

	ret = mpm_parse_dt();
	if (ret < 0)
		return -ENODEV;

	print_boot_stats();

	iounmap(boot_stats);
	iounmap(mpm_counter_base);

	return 0;
}

