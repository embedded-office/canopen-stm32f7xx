/******************************************************************************
   Copyright 2020 Embedded Office GmbH & Co. KG

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
******************************************************************************/

/******************************************************************************
* INCLUDES
******************************************************************************/

#include "drv_can_stm32f7xx.h"
#include "stm32f7xx_hal.h"

/******************************************************************************
* PRIVATE TYPE DEFINITION
******************************************************************************/

typedef struct BAUDRATE_TBL_T {
    uint32_t Baudrate;
    uint32_t Prescaler;
    uint32_t SyncJumpWidth;
    uint32_t TimeSeg1;
    uint32_t TimeSeg2;

} BAUDRATE_TBL;

/******************************************************************************
* PRIVATE VARIABLES
******************************************************************************/

CAN_HandleTypeDef CanBus;

/******************************************************************************
* PRIVATE FUNCTIONS
******************************************************************************/

static void    DrvCanInit   (void);
static void    DrvCanEnable (uint32_t baudrate);
static int16_t DrvCanSend   (CO_IF_FRM *frm);
static int16_t DrvCanRead   (CO_IF_FRM *frm);
static void    DrvCanReset  (void);
static void    DrvCanClose  (void);

/******************************************************************************
* PUBLIC VARIABLE
******************************************************************************/

const CO_IF_CAN_DRV STM32F7xxCanDriver = {
    DrvCanInit,
    DrvCanEnable,
    DrvCanRead,
    DrvCanSend,
    DrvCanReset,
    DrvCanClose
};

BAUDRATE_TBL BaudrateTbl[] = {
    {   10000, 300, CAN_SJW_1TQ, CAN_BS1_15TQ, CAN_BS2_2TQ },  /* SP: 88,9%, ERR:     0% */
    {   20000, 150, CAN_SJW_1TQ, CAN_BS1_15TQ, CAN_BS2_2TQ },  /* SP: 88,9%, ERR:     0% */
    {   50000,  60, CAN_SJW_1TQ, CAN_BS1_15TQ, CAN_BS2_2TQ },  /* SP: 88,9%, ERR:     0% */
    {  125000,  24, CAN_SJW_1TQ, CAN_BS1_15TQ, CAN_BS2_2TQ },  /* SP: 88,9%, ERR:     0% */
    {  250000,  12, CAN_SJW_1TQ, CAN_BS1_15TQ, CAN_BS2_2TQ },  /* SP: 88,9%, ERR:     0% */
    {  500000,   6, CAN_SJW_1TQ, CAN_BS1_15TQ, CAN_BS2_2TQ },  /* SP: 88,9%, ERR:     0% */
    {  800000,   4, CAN_SJW_1TQ, CAN_BS1_13TQ, CAN_BS2_3TQ },  /* SP: 82,4%, ERR: -0,74% */
    { 1000000,   3, CAN_SJW_1TQ, CAN_BS1_15TQ, CAN_BS2_2TQ },  /* SP: 88,9%, ERR:     0% */
    { 0, 0, 0, 0, 0 }
};

/******************************************************************************
* PUBLIC FUNCTIONS
******************************************************************************/

/* ST HAL CAN Receive Interrupt Handler */
void CAN1_RX0_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&CanBus);
}

/******************************************************************************
* PRIVATE FUNCTIONS
******************************************************************************/

static void DrvCanInit(void)
{
    GPIO_InitTypeDef gpio = {0};

    /* Peripheral clocks enable */
    __HAL_RCC_CAN1_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* CAN1 GPIO Config: PB8 -> CAN1_RX, PB9 -> CAN1_TX */
    gpio.Pin       = GPIO_PIN_8|GPIO_PIN_9;
    gpio.Mode      = GPIO_MODE_AF_PP;
    gpio.Pull      = GPIO_NOPULL;
    gpio.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOB, &gpio);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
}

static void DrvCanEnable(uint32_t baudrate)
{
    uint8_t idx = 0;

    /* find the given baudrate in baudrate table */
    while (BaudrateTbl[idx].Baudrate != 0) {
        if (baudrate == BaudrateTbl[idx].Baudrate) {
            break;
        }
        idx++;
    }
    if (baudrate != BaudrateTbl[idx].Baudrate) {
        while(1) {;}      /* error not handled */
    }

    /* can controller mode */
    CanBus.Instance  = CAN1;
    CanBus.Init.Mode = CAN_MODE_NORMAL;

    /* baudrate settings */
    CanBus.Init.Prescaler     = BaudrateTbl[idx].Prescaler;
    CanBus.Init.SyncJumpWidth = BaudrateTbl[idx].SyncJumpWidth;
    CanBus.Init.TimeSeg1      = BaudrateTbl[idx].TimeSeg1;
    CanBus.Init.TimeSeg2      = BaudrateTbl[idx].TimeSeg2;

    /* feature select */
    CanBus.Init.TimeTriggeredMode    = DISABLE;
    CanBus.Init.AutoBusOff           = DISABLE;
    CanBus.Init.AutoWakeUp           = DISABLE;
    CanBus.Init.AutoRetransmission   = DISABLE;
    CanBus.Init.ReceiveFifoLocked    = DISABLE;
    CanBus.Init.TransmitFifoPriority = DISABLE;
    HAL_CAN_Init(&CanBus);
    HAL_CAN_Start(&CanBus);
}

static int16_t DrvCanSend(CO_IF_FRM *frm)
{
    HAL_StatusTypeDef   result;
    CAN_TxHeaderTypeDef frmHead;
    uint32_t            mailbox;

    /* RTR is not supported */
    frmHead.RTR   = 0;

    /* extended identifiers are not supported */
    frmHead.ExtId = 0;
    frmHead.IDE   = 0;

    /* fill identifier, DLC and data payload in transmit buffer */
    frmHead.StdId = frm->Identifier;
    frmHead.DLC   = frm->DLC;
    result = HAL_CAN_AddTxMessage(&CanBus, &frmHead, &frm->Data[0], &mailbox);
    if (result != HAL_OK) {
        return (-1);
    }
    return (0u);
}

static int16_t DrvCanRead (CO_IF_FRM *frm)
{
    HAL_StatusTypeDef err;
    CAN_RxHeaderTypeDef frmHead;
    uint8_t frmData[8] = { 0 };
    uint8_t n;

    err = HAL_CAN_GetRxMessage(&CanBus, CAN_RX_FIFO0, &frmHead, &frmData[0]);
    if (err != HAL_OK) {
        return (-1);
    }

    /* fill CAN frame on success */
    frm->Identifier  = frmHead.StdId;
    frm->DLC         = frmHead.DLC;
    for (n = 0; n < 8; n++) {
        frm->Data[n] = frmData[n];
    }
    return (frm->DLC);
}

static void DrvCanReset(void)
{
    HAL_CAN_Init(&CanBus);
    HAL_CAN_Start(&CanBus);
}

static void DrvCanClose(void)
{
    HAL_CAN_Stop(&CanBus);
}
