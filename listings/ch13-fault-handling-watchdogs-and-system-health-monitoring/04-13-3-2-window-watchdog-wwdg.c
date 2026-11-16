/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 13: Fault Handling, Watchdogs, and System Health Monitoring
 * 13.3 Watchdog Timers > 13.3.2 Window Watchdog (WWDG)
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
void wwdg_init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_WWDGEN;

    /* Prescaler /8, window = 0x50, counter start = 0x7F */
    WWDG->CFR = WWDG_CFR_EWI            /* Enable early wakeup IRQ */
              | WWDG_CFR_WDGTB_0
              | WWDG_CFR_WDGTB_1        /* WDGTB = 11: prescaler /8 */
              | (0x50 & 0x7F);          /* Window value            */

    WWDG->CR  = WWDG_CR_WDGA            /* Activate WWDG           */
              | (0x7F & 0x7F);          /* Counter = 0x7F          */

    NVIC_SetPriority(WWDG_IRQn, 0);     /* Highest priority        */
    NVIC_EnableIRQ(WWDG_IRQn);
}

void WWDG_IRQHandler(void)
{
    WWDG->SR = 0;                        /* Clear EWI flag          */
    /* Last chance: log diagnostic data before reset */
    fault_log_to_backup_sram(0, 0, 0xDEAD0001, 0);
}
