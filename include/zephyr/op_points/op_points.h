/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_SUBSYS_OP_POINTS_H_
#define ZEPHYR_SUBSYS_OP_POINTS_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Operating Points
 * @defgroup subsys_op_points Operating Points (OpPoints)
 * @since 4.5
 * @version 0.1.0
 * @ingroup os_services
 * @{
 */

struct op_point {
	const struct pstate **pstates;
	size_t pstates_len;
	atomic_val_t *pstates_mask;
	const struct device **devices;
	size_t devices_len;
};

/** Synthesize symbol of op-point from devicetree dependency ordinal */
#define OP_POINT_DT_SYM(_node) _CONCAT(__op_point_, DT_DEP_ORD(_node))

/**
 * @brief Get a op-point reference from a devicetree node identifier.
 *
 * To be used in DT_FOREACH_CHILD() or similar macros
 *
 * @param _node Node identifier.
 */
#define OP_POINT_DT_GET(_node) &OP_POINT_DT_SYM(_node)

/* Define op-points as extern to be picked up by API users */
#define Z_DECLARE_OP_POINT_EXTERN(_node) extern const struct op_point OP_POINT_DT_SYM(_node);

DT_FOREACH_CHILD_STATUS_OKAY(DT_INST(0, zephyr_op_points), Z_DECLARE_OP_POINT_EXTERN)

/**
 * @brief Activate a given operating point.
 *
 * Activate a given operating point. The available p-states will be filtered and any devices
 * associated to the operating point will be resumed/runtime fetched. Any devices that were
 * in use by the previously active operating point will be suspended/runtime put if not present
 * in the newly activated operating point
 *
 * @note Any updates to the p-state will occur the next time the CPU Freq subsystem updates.
 *
 * @return 0 if request received successfully, -errno in case of failure.
 */
int op_point_activate(const struct op_point *point);

/**
 * @brief Clear the currently selected operating point.
 *
 * Clear any previously activated operating points. No p-states will be filtered and any devices
 * previously resumed/runtime enabled will be suspended/released.
 *
 * @note Any updates to the p-state will occur the next time the CPU Freq subsystem updates.
 *
 * @return 0 if request received successfully, -errno in case of failure.
 */
int op_point_clear(void);

/**
 * @}
 */
#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_SUBSYS_OP_POINTS_H_ */
