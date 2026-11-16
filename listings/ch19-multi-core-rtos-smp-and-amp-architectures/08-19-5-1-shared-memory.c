/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 19: Multi-Core RTOS: SMP and AMP Architectures
 * 19.5 Inter-Core Communication > 19.5.1 Shared Memory
 * Verification: executed (helper functions called with test arguments)
 * SPDX-License-Identifier: MIT
 */
void mpu_configure_shared_region(uint32_t base, uint32_t size)
{
    MPU_Region_InitTypeDef mpu_init = {0};
    HAL_MPU_Disable();

    mpu_init.Enable           = MPU_REGION_ENABLE;
    mpu_init.BaseAddress      = base;
    mpu_init.Size             = MPU_REGION_SIZE_32KB;
    mpu_init.AccessPermission = MPU_REGION_FULL_ACCESS;
    mpu_init.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
    mpu_init.IsCacheable      = MPU_ACCESS_NOT_CACHEABLE;
    mpu_init.IsShareable      = MPU_ACCESS_SHAREABLE;
    mpu_init.TypeExtField     = MPU_TEX_LEVEL1;
    mpu_init.Number           = MPU_REGION_NUMBER5;
    mpu_init.SubRegionDisable = 0x00;
    mpu_init.DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE;
    HAL_MPU_ConfigRegion(&mpu_init);

    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}
