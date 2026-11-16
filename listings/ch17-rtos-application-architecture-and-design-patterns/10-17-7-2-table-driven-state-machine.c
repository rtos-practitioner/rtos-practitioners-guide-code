/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 17: RTOS Application Architecture and Design Patterns
 * 17.7 State Machine Patterns > 17.7.2 Table-Driven State Machine
 * Verification: executed (helper functions called with test arguments)
 * SPDX-License-Identifier: MIT
 */
typedef enum {
    MOTOR_IDLE, MOTOR_STARTING, MOTOR_RUNNING,
    MOTOR_STOPPING, MOTOR_FAULT,
    MOTOR_STATE_COUNT
} MotorState_t;

typedef enum {
    ME_START_CMD, ME_SPEED_OK, ME_STOP_CMD,
    ME_FAULT_DETECTED, ME_FAULT_CLEARED, ME_TIMEOUT,
    MOTOR_EVENT_COUNT
} MotorEvent_t;

typedef void (*ActionFunc_t)(void);
typedef bool (*GuardFunc_t)(void);

typedef struct {
    MotorState_t  eNextState;
    ActionFunc_t  pfAction;      /* NULL = no action */
    GuardFunc_t   pfGuard;       /* NULL = always true */
    bool          bValid;        /* false = no transition defined */
} Transition_t;

/* --- Actions --- */
static void vActionEnergizeCoils(void)  { /* PWM start */  }
static void vActionSetRunSpeed(void)    { /* PID engage */ }
static void vActionDeenergize(void)     { /* PWM stop */   }
static void vActionLogFault(void)       { /* Log error */  }
static void vActionResetFault(void)     { /* Clear latch */}

/* --- Guard conditions --- */
static bool bGuardTempOk(void)
{
    return (ulReadTemperature() < TEMP_THRESHOLD);
}

/* --- Transition table: [state][event] --- */
static const Transition_t xTransTable[MOTOR_STATE_COUNT]
                                     [MOTOR_EVENT_COUNT] = {
    /* MOTOR_IDLE */
    [MOTOR_IDLE][ME_START_CMD] =
        { MOTOR_STARTING, vActionEnergizeCoils, bGuardTempOk, true },
    [MOTOR_IDLE][ME_FAULT_DETECTED] =
        { MOTOR_FAULT,    vActionLogFault,      NULL,         true },

    /* MOTOR_STARTING */
    [MOTOR_STARTING][ME_SPEED_OK] =
        { MOTOR_RUNNING,  vActionSetRunSpeed,   NULL,         true },
    [MOTOR_STARTING][ME_TIMEOUT] =
        { MOTOR_FAULT,    vActionLogFault,      NULL,         true },
    [MOTOR_STARTING][ME_FAULT_DETECTED] =
        { MOTOR_FAULT,    vActionLogFault,      NULL,         true },

    /* MOTOR_RUNNING */
    [MOTOR_RUNNING][ME_STOP_CMD] =
        { MOTOR_STOPPING, vActionDeenergize,    NULL,         true },
    [MOTOR_RUNNING][ME_FAULT_DETECTED] =
        { MOTOR_FAULT,    vActionLogFault,      NULL,         true },

    /* MOTOR_STOPPING */
    [MOTOR_STOPPING][ME_SPEED_OK] =    /* speed = 0 */
        { MOTOR_IDLE,     NULL,                 NULL,         true },
    [MOTOR_STOPPING][ME_TIMEOUT] =
        { MOTOR_FAULT,    vActionLogFault,      NULL,         true },

    /* MOTOR_FAULT */
    [MOTOR_FAULT][ME_FAULT_CLEARED] =
        { MOTOR_IDLE,     vActionResetFault,    NULL,         true },
};

/* --- Generic engine --- */
static MotorState_t eCurrentState = MOTOR_IDLE;

void vMotorStateMachineProcess(MotorEvent_t eEvent)
{
    if (eEvent >= MOTOR_EVENT_COUNT) {
        return;  /* Reject out-of-range events */
    }

    const Transition_t *pxTrans =
        &xTransTable[eCurrentState][eEvent];

    /* Unlisted cells are zero-initialized, so bValid is false.
       eNextState cannot be the test: MOTOR_IDLE is also 0. */
    if (!pxTrans->bValid) {
        return;  /* No transition defined for this event */
    }

    /* Evaluate guard condition */
    if (pxTrans->pfGuard != NULL && !pxTrans->pfGuard()) {
        return;  /* Guard blocks transition */
    }

    /* Execute action */
    if (pxTrans->pfAction != NULL) {
        pxTrans->pfAction();
    }

    /* Transition to next state */
    eCurrentState = pxTrans->eNextState;
}
