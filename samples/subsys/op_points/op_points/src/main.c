/*
 * Copyright 2026 NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdint.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/op_points/op_points.h>

const struct op_point *perf = OP_POINT_DT_GET(DT_NODELABEL(perf));

const struct op_point *nom = OP_POINT_DT_GET(DT_NODELABEL(nom));

int main(void)
{

	printk("Op-Points sample\n");

	while (1) {
		k_sleep(K_SECONDS(2));
		printk("Setting op-point: nominal\n");
		op_point_activate(nom);

		k_sleep(K_SECONDS(2));
		printk("Setting op-point: performance\n");
		op_point_activate(perf);

		k_sleep(K_SECONDS(2));
		printk("Clearing op-point\n");
		op_point_clear();
	}
}
