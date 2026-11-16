/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 9: Device Driver Architecture and Peripheral Abstraction
 * 9.6 CAN Bus Driver > 9.6.3 Implementation
 * Verification: executed (helper functions called with test arguments)
 * SPDX-License-Identifier: MIT
 */
/* can_driver.c */
#include "can_driver.h"
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

static CAN_HandleTypeDef    hcan1;
static QueueHandle_t        can_rx_queue;
static SemaphoreHandle_t    can_tx_sem;

#define CAN_RX_QUEUE_LEN    32

int can_init(uint32_t bitrate)
{
    /* Bit time = 1 (sync) + 11 (BS1) + 2 (BS2) = 14 tq,
     * sample point = 12/14 = 85.7%. CAN1 is clocked from APB1 (42 MHz),
     * so valid bitrates divide 3 MHz: 1 Mbit/s, 500k, 250k, 125k... */
    const uint32_t tq_per_bit = 14U;
    uint32_t pclk1 = HAL_RCC_GetPCLK1Freq();

    if (bitrate == 0 || (pclk1 % (bitrate * tq_per_bit)) != 0) {
        return -1;  /* Bitrate not exactly achievable */
    }
    uint32_t prescaler = pclk1 / (bitrate * tq_per_bit);
    if (prescaler < 1 || prescaler > 1024) return -1;

    can_rx_queue = xQueueCreate(CAN_RX_QUEUE_LEN, sizeof(can_msg_t));
    can_tx_sem   = xSemaphoreCreateCounting(3, 3);  /* 3 TX mailboxes */

    if (!can_rx_queue || !can_tx_sem) return -1;

    __HAL_RCC_CAN1_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    /* PD0 = CAN1_RX, PD1 = CAN1_TX (AF9) */
    GPIO_InitTypeDef gpio = {0};
    gpio.Pin       = GPIO_PIN_0 | GPIO_PIN_1;
    gpio.Mode      = GPIO_MODE_AF_PP;
    gpio.Pull      = GPIO_NOPULL;
    gpio.Speed     = GPIO_SPEED_FREQ_HIGH;
    gpio.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOD, &gpio);

    hcan1.Instance                  = CAN1;
    hcan1.Init.Prescaler            = prescaler;
    hcan1.Init.Mode                 = CAN_MODE_NORMAL;
    hcan1.Init.SyncJumpWidth        = CAN_SJW_1TQ;
    hcan1.Init.TimeSeg1             = CAN_BS1_11TQ;
    hcan1.Init.TimeSeg2             = CAN_BS2_2TQ;
    hcan1.Init.AutoBusOff           = ENABLE;
    hcan1.Init.AutoRetransmission   = ENABLE;
    hcan1.Init.AutoWakeUp           = DISABLE;
    hcan1.Init.TransmitFifoPriority = DISABLE;

    if (HAL_CAN_Init(&hcan1) != HAL_OK) return -1;

    /* Enable RX FIFO0 message pending interrupt */
    HAL_CAN_ActivateNotification(&hcan1,
                                  CAN_IT_RX_FIFO0_MSG_PENDING);
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_TX_MAILBOX_EMPTY);

    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
    HAL_NVIC_SetPriority(CAN1_TX_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);

    HAL_CAN_Start(&hcan1);
    return 0;
}

/* Standard (11-bit) ID filter; in 32-bit scale STID occupies bits 31:21 */
int can_add_filter(uint32_t id, uint32_t mask)
{
    CAN_FilterTypeDef filter = {0};
    static uint8_t filter_bank = 0;

    filter.FilterBank           = filter_bank++;
    filter.FilterMode           = CAN_FILTERMODE_IDMASK;
    filter.FilterScale          = CAN_FILTERSCALE_32BIT;
    filter.FilterIdHigh         = (id << 5) & 0xFFFF;
    filter.FilterIdLow          = 0;
    filter.FilterMaskIdHigh     = (mask << 5) & 0xFFFF;
    filter.FilterMaskIdLow      = 0;
    filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    filter.FilterActivation     = ENABLE;
    filter.SlaveStartFilterBank = 14;   /* Banks 0-13 for CAN1 */

    return (HAL_CAN_ConfigFilter(&hcan1, &filter) == HAL_OK) ? 0 : -1;
}

/* RX FIFO0 callback -- called from CAN1_RX0_IRQHandler */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    CAN_RxHeaderTypeDef header;
    can_msg_t msg;

    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0,
                              &header, msg.data) == HAL_OK) {
        msg.id_type = (header.IDE == CAN_ID_EXT) ? 1 : 0;
        msg.id      = msg.id_type ? header.ExtId : header.StdId;
        msg.dlc     = header.DLC;

        xQueueSendFromISR(can_rx_queue, &msg,
                           &xHigherPriorityTaskWoken);
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/* TX mailbox released (sent or aborted) -- return it to the pool */
static void can_tx_mailbox_released(void)
{
    BaseType_t xWoken = pdFALSE;
    xSemaphoreGiveFromISR(can_tx_sem, &xWoken);
    portYIELD_FROM_ISR(xWoken);
}

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{ (void)hcan; can_tx_mailbox_released(); }
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan)
{ (void)hcan; can_tx_mailbox_released(); }
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan)
{ (void)hcan; can_tx_mailbox_released(); }
void HAL_CAN_TxMailbox0AbortCallback(CAN_HandleTypeDef *hcan)
{ (void)hcan; can_tx_mailbox_released(); }
void HAL_CAN_TxMailbox1AbortCallback(CAN_HandleTypeDef *hcan)
{ (void)hcan; can_tx_mailbox_released(); }
void HAL_CAN_TxMailbox2AbortCallback(CAN_HandleTypeDef *hcan)
{ (void)hcan; can_tx_mailbox_released(); }

int can_send(const can_msg_t *msg, uint32_t timeout_ms)
{
    if (xSemaphoreTake(can_tx_sem,
                        pdMS_TO_TICKS(timeout_ms)) != pdTRUE) {
        return -1;  /* No mailbox available */
    }

    CAN_TxHeaderTypeDef header = {0};
    if (msg->id_type) {
        header.ExtId = msg->id;
        header.IDE   = CAN_ID_EXT;
    } else {
        header.StdId = msg->id;
        header.IDE   = CAN_ID_STD;
    }
    header.RTR   = CAN_RTR_DATA;
    header.DLC   = msg->dlc;

    uint32_t mailbox;
    if (HAL_CAN_AddTxMessage(&hcan1, &header,
                              (uint8_t *)msg->data,
                              &mailbox) != HAL_OK) {
        xSemaphoreGive(can_tx_sem);
        return -1;
    }
    return 0;
}

int can_receive(can_msg_t *msg, uint32_t timeout_ms)
{
    if (xQueueReceive(can_rx_queue, msg,
                       pdMS_TO_TICKS(timeout_ms)) == pdTRUE) {
        return 0;
    }
    return -1;  /* Timeout */
}
