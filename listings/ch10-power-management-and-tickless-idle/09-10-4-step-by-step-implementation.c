/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 10: Power Management and Tickless Idle
 * 10.4 Custom Tickless Implementation for Stop Mode > Step-by-Step Implementation
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
/* Wakeup timer: RTCCLK/16 = 2048 Hz, ~488 us per count */
#define RTC_WUT_HZ             2048U
#define MAX_RTC_WAKEUP_COUNT   0x10000U  /* WUTR is 16 bits: 1..65536 counts */
#define RTC_SUBSEC_PER_SEC     (RTC_PREDIV_S + 1U)
#define RTC_SUBSEC_PER_DAY     (86400U * RTC_SUBSEC_PER_SEC)

/* Time of day in sub-second units. Reading SSR locks the TR/DR shadow
 * registers until DR is read, so the read order is SSR, TR, DR. */
static uint32_t RTC_ReadSubsecOfDay(void)
{
    uint32_t ssr = RTC->SSR;
    uint32_t tr  = RTC->TR;
    (void)RTC->DR;

    uint32_t hours = ((tr >> 20) & 0x3U) * 10U + ((tr >> 16) & 0xFU);
    uint32_t mins  = ((tr >> 12) & 0x7U) * 10U + ((tr >> 8)  & 0xFU);
    uint32_t secs  = ((tr >> 4)  & 0x7U) * 10U + (tr & 0xFU);

    /* SSR counts down from PREDIV_S: elapsed fraction = PREDIV_S - SSR */
    return (hours * 3600U + mins * 60U + secs) * RTC_SUBSEC_PER_SEC
           + (RTC_PREDIV_S - ssr);
}

void App_EnterTicklessStop(TickType_t xExpectedIdleTime)
{
    TickType_t  xMaxSuppressedTicks;
    uint32_t    ulWakeupCount;
    uint32_t    ulTimeBeforeSleep;
    uint32_t    ulTimeAfterSleep;
    uint32_t    ulElapsedSubsec;
    uint32_t    ulWokenByTimer;
    TickType_t  xActualIdleTicks;
    eSleepModeStatus eSleepStatus;

    /* Cap to the maximum the RTC wakeup timer can handle (32 s) */
    xMaxSuppressedTicks = (TickType_t)(((uint64_t)MAX_RTC_WAKEUP_COUNT
                                        * configTICK_RATE_HZ) / RTC_WUT_HZ);
    if (xExpectedIdleTime > xMaxSuppressedTicks)
        xExpectedIdleTime = xMaxSuppressedTicks;

    /* Convert ticks to wakeup-timer counts, rounded to nearest count */
    ulWakeupCount = (uint32_t)(((uint64_t)xExpectedIdleTime * RTC_WUT_HZ
                                + configTICK_RATE_HZ / 2U) / configTICK_RATE_HZ);
    if (ulWakeupCount == 0)
        return;

    /* Disable SysTick interrupt --- we are taking over timekeeping */
    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;

    /* Confirm it is still safe to sleep */
    __disable_irq();
    eSleepStatus = eTaskConfirmSleepModeStatus();

    if (eSleepStatus == eAbortSleep)
    {
        /* A task was made ready; abort sleep */
        SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
        __enable_irq();
        return;
    }

    /* Program RTC wakeup timer */
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;
    RTC->CR &= ~RTC_CR_WUTE;                 /* Disable to write      */
    while (!(RTC->ISR & RTC_ISR_WUTWF))
        ;
    RTC->WUTR = ulWakeupCount - 1;            /* Countdown value       */
    RTC->ISR &= ~RTC_ISR_WUTF;               /* Clear wakeup flag     */
    RTC->CR  |= RTC_CR_WUTE | RTC_CR_WUTIE;  /* Enable timer + IRQ    */
    RTC->WPR  = 0xFF;
    EXTI->PR  = (1U << 22);                   /* Clear pending EXTI    */

    /* Record calendar time before sleep */
    ulTimeBeforeSleep = RTC_ReadSubsecOfDay();

    /* --- Enter Stop Mode --- */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    PWR->CR  &= ~PWR_CR_PDDS;
    PWR->CR  |= PWR_CR_LPDS;
    __DSB();
    __WFI();

    /* --- Woken up --- */
    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;

    /* Restore system clocks immediately */
    SystemClock_RestoreAfterStop();

    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    /* Shadow registers are stale after Stop: resynchronize before reading */
    RTC->ISR &= ~RTC_ISR_RSF;
    while (!(RTC->ISR & RTC_ISR_RSF))
        ;
    ulTimeAfterSleep = RTC_ReadSubsecOfDay();
    ulWokenByTimer   = RTC->ISR & RTC_ISR_WUTF;

    /* Disable RTC wakeup timer (pending IRQ handler clears WUTF/EXTI) */
    RTC->CR &= ~(RTC_CR_WUTE | RTC_CR_WUTIE);
    RTC->WPR = 0xFF;

    __enable_irq();

    if (ulWokenByTimer)
    {
        /* Wakeup timer expired: the full expected period elapsed */
        xActualIdleTicks = xExpectedIdleTime;
    }
    else
    {
        /* Early wakeup: measure from calendar, handling midnight wrap */
        if (ulTimeAfterSleep >= ulTimeBeforeSleep)
            ulElapsedSubsec = ulTimeAfterSleep - ulTimeBeforeSleep;
        else
            ulElapsedSubsec = RTC_SUBSEC_PER_DAY - ulTimeBeforeSleep
                              + ulTimeAfterSleep;

        xActualIdleTicks = (TickType_t)(((uint64_t)ulElapsedSubsec
                                         * configTICK_RATE_HZ)
                                        / RTC_SUBSEC_PER_SEC);
        if (xActualIdleTicks > xExpectedIdleTime)
            xActualIdleTicks = xExpectedIdleTime;
    }

    /* Compensate the RTOS tick count */
    vTaskStepTick(xActualIdleTicks);

    /* Re-enable SysTick for normal operation */
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
}
