/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/cpu_freq/pstate.h>
#include <zephyr/cpu_freq/cpu_freq.h>
#include <zephyr/op_points/op_points.h>
#include <zephyr/pm/device.h>
#include <zephyr/pm/device_runtime.h>
#include <zephyr/sys/atomic.h>
#include <zephyr/device.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(op_points, CONFIG_OP_POINTS_LOG_LEVEL);

#define DT_DRV_COMPAT zephyr_op_points

#define OP_POINTS_PSTATES_REF(node_id) op_points_pstates##node_id
#define OP_POINTS_DEVICES_REF(node_id) op_points_devices##node_id

#define OP_POINT_PSTATE_REF(node, prop, idx) PSTATE_DT_GET(DT_PHANDLE_BY_IDX(node, prop, idx))

#define OP_POINT_PSTATES(child)                                                                    \
	static const struct pstate *OP_POINTS_PSTATES_REF(child)[] = {                             \
		DT_FOREACH_PROP_ELEM_SEP(child, p_states, OP_POINT_PSTATE_REF, (,)) };

#define OP_POINT_DEVICE_REF(node, prop, idx) DEVICE_DT_GET(DT_PHANDLE_BY_IDX(node, prop, idx))

#define OP_POINT_DEVICES(child)                                                                    \
	static const struct device *OP_POINTS_DEVICES_REF(child)[] = {                             \
		COND_CODE_1(DT_NODE_HAS_PROP(child, devices),                                      \
			    (DT_FOREACH_PROP_ELEM_SEP(child, devices, OP_POINT_DEVICE_REF, (,))),  \
			    ()) };

DT_INST_FOREACH_CHILD_STATUS_OKAY(0, OP_POINT_PSTATES);
DT_INST_FOREACH_CHILD_STATUS_OKAY(0, OP_POINT_DEVICES);

#define OP_POINT_PSTATES_MASK_SYM(node_id) _CONCAT(OP_POINT_DT_SYM(node_id), pstate_mask)
#define OP_POINT(node_id)                                                                          \
	const struct op_point OP_POINT_DT_SYM(node_id) = (struct op_point){                        \
		.pstates = OP_POINTS_PSTATES_REF(node_id),                                         \
		.pstates_len = DT_PROP_LEN(node_id, p_states),                                     \
		.devices = OP_POINTS_DEVICES_REF(node_id),                                         \
		.devices_len = DT_PROP_LEN_OR(node_id, devices, 0),                                \
		.pstates_mask = &OP_POINT_PSTATES_MASK_SYM(node_id),                               \
	};

#define OP_POINT_PSTATE_MASK(node_id) static atomic_val_t OP_POINT_PSTATES_MASK_SYM(node_id);

static const struct pstate *const op_points_pstates[] = {
	DT_FOREACH_CHILD_STATUS_OKAY_SEP(DT_PATH(performance_states), PSTATE_DT_GET, (,))};

DT_INST_FOREACH_CHILD_STATUS_OKAY(0, OP_POINT_PSTATE_MASK);
DT_INST_FOREACH_CHILD_STATUS_OKAY(0, OP_POINT);

static const struct op_point *active_point;
static atomic_t pstate_mask;

#if defined(CONFIG_PM_DEVICE)

static bool device_in_op_point(const struct op_point *op, const struct device *dev)
{
	if (!op) {
		return false;
	}

	for (size_t i = 0; i < op->devices_len; i++) {
		if (op->devices[i] == dev) {
			return true;
		}
	}

	return false;
}

static int process_devices(const struct op_point *op, const struct op_point *prev)
{
	int ret;

	if (prev) {
		for (size_t i = 0; i < prev->devices_len; i++) {
			if (!device_in_op_point(op, prev->devices[i])) {
				if (IS_ENABLED(CONFIG_PM_DEVICE_RUNTIME)) {
					ret = pm_device_runtime_put(prev->devices[i]);
				} else {
					ret = pm_device_action_run(prev->devices[i],
								   PM_DEVICE_ACTION_SUSPEND);
				}

				if (ret && (ret != -EALREADY) && (ret != -ENOSYS)) {
					LOG_ERR("Failed to release device %s from previously "
						"active op-point: %d",
						prev->devices[i]->name, ret);
					return ret;
				}
			}
		}
	}
	if (op) {
		for (size_t i = 0; i < op->devices_len; i++) {
			if (!device_in_op_point(prev, op->devices[i])) {
				if (IS_ENABLED(CONFIG_PM_DEVICE_RUNTIME)) {
					ret = pm_device_runtime_get(op->devices[i]);
				} else {
					ret = pm_device_action_run(op->devices[i],
								   PM_DEVICE_ACTION_RESUME);
				}

				if (ret && (ret != -EALREADY) && (ret != -ENOSYS)) {
					LOG_ERR("Failed to get/resume device %s for new op-point: "
						"%d",
						op->devices[i]->name, ret);
					return ret;
				}
			}
		}
	}

	return 0;
}

#else

#define process_devices(_point, _prev) 0

#endif

static int activate(const struct op_point *point)
{
	const struct op_point *prev;

	if (active_point == point) {
		return 0;
	}

	prev = active_point;
	active_point = point;

	if (active_point) {
		atomic_set(&pstate_mask, *active_point->pstates_mask);
	} else {
		atomic_set(&pstate_mask, 0);
	}

	return process_devices(active_point, prev);
}

int op_point_activate(const struct op_point *point)
{
	if (!point) {
		return -EINVAL;
	}

	return activate(point);
}

int op_point_clear(void)
{
	return activate(NULL);
}

static atomic_val_t op_points_pstates_constraints(void)
{
	return atomic_get(&pstate_mask);
}

CPU_FREQ_CONSTRAINT_DEFINE(op_points, 0, op_points_pstates_constraints);

#define INIT_OP_POINT_PSTATES_MASK(node_id) op_points_init_pstates_mask(&OP_POINT_DT_SYM(node_id));

static uint8_t pstate_index(const struct pstate *pstate)
{
	for (size_t i = 0; i < ARRAY_SIZE(op_points_pstates); i++) {
		if (op_points_pstates[i] == pstate) {
			return i;
		}
	}

	__ASSERT(0, "Invalid pstate pointer");
	return 0;
}

static void op_points_init_pstates_mask(const struct op_point *op)
{
	atomic_val_t mask = 0;

	for (size_t i = 0; i < op->pstates_len; i++) {
		mask |= pstate_index(op->pstates[i]);
	}

	*op->pstates_mask = ~mask;
}

static int op_points_init(void)
{
	DT_INST_FOREACH_CHILD_STATUS_OKAY(0, INIT_OP_POINT_PSTATES_MASK)

	return 0;
}

SYS_INIT(op_points_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
