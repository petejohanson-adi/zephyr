/*
 * Copyright (c) 2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @ingroup subsys_cpu_freq
 * @brief Main header file for CPU frequency scaling API.
 */

#ifndef ZEPHYR_INCLUDE_CPU_FREQ_CPU_FREQ_H_
#define ZEPHYR_INCLUDE_CPU_FREQ_CPU_FREQ_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr/types.h>
#include <zephyr/cpu_freq/pstate.h>

/**
 * @brief Dynamic CPU Frequency Scaling
 * @defgroup subsys_cpu_freq CPU Frequency (CPUFreq)
 * @since 4.3
 * @version 0.1.0
 * @ingroup os_services
 * @{
 */

/**
 * @brief Request processor set the given performance state.
 *
 * To be implemented by the SoC. This API abstracts the hardware and SoC
 * specific calls required to change the performance state of the current CPU.
 * The caller must ensure that the current CPU does not change. If called from
 * an ISR or a single CPU system, this restriction is automatically met. If
 * called from a thread on an SMP system, either interrupts or the scheduler
 * must be disabled to ensure the current CPU does not change.
 *
 * @note It is not guaranteed that the performance state will be set immediately, or at all.
 *
 * @param state Pointer to performance state.
 *
 *
 * @return 0 if request received successfully, -errno in case of failure.
 */
int cpu_freq_pstate_set(const struct pstate *state);

/**
 * TODO: DOCS!
 */

typedef const struct pstate* (*cpu_freq_constraint_func_t)(const struct pstate *state);

/**
 * @brief SoC-specific PMP region descriptor.
 *
 * SoCs can define additional memory regions that need PMP protection
 * using the PMP_SOC_REGION_DEFINE macro.
 * These regions are automatically collected via iterable sections and
 * programmed into the PMP during initialization.
 *
 * Note: Uses start/end pointers instead of start/size to support regions
 * defined by linker symbols where the size is not a compile-time constant.
 */
struct cpu_freq_constraint {
	/** Constraint function */
	cpu_freq_constraint_func_t func;
};

/**
 * @brief Define a CPU Frequency Constraint.
 *
 * TODO: Add docs
 *
 * @param name Unique identifier for this region
 * @param _priority Priority for the constraint, in case multiple apply
 * @param _func The callback for applying the constraint
 */
#define CPU_FREQ_CONSTRAINT_DEFINE(name, _priority, _func)				\
	static const STRUCT_SECTION_ITERABLE_NAMED(cpu_freq_constraint,			\
						   _CONCAT(_priority, name), name) = {	\
		.func = _func,									\
	}

/**
 * @brief Iterate the CPU Frequency Constraints.
 *
 * TODO: Add docs
 *
 * @param iterator Pointer iterator for the constraint functions
 */
#define CPU_FREQ_CONSTRAINTS_FOREACH(iterator) \
	STRUCT_SECTION_FOREACH(cpu_freq_constraint, iterator)
/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_CPU_FREQ_CPU_FREQ_H_ */
