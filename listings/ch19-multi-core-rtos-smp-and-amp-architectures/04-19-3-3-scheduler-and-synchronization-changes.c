/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 19: Multi-Core RTOS: SMP and AMP Architectures
 * 19.3 FreeRTOS SMP > 19.3.3 Scheduler and Synchronization Changes
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
#include "hardware/sync.h"

static spin_lock_t *pxCounterLock;

void counter_init(void)
{
    pxCounterLock = spin_lock_init(spin_lock_claim_unused(true));
}

void counter_increment(void)
{
    /* Masks local interrupts and spins for the lock */
    uint32_t ulSaved = spin_lock_blocking(pxCounterLock);
    shared_counter++;
    spin_unlock(pxCounterLock, ulSaved);
}
