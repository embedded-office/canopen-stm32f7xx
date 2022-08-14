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

#include "drv_can2_stm32f7xx.h"
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

typedef struct PIN_ASSIGN_T {
    GPIO_TypeDef *Port;
    uint16_t      Pin; 
} PIN_ASSIGN;

/******************************************************************************
* PRIVATE DEFINES
******************************************************************************/

/* default pin assignment: CAN_RX -> PB5, CAN_TX -> PB6 */
#define CAN2_PIN_RX_SEL  0
#define CAN2_PIN_TX_SEL  0

/******************************************************************************
* PRIVATE VARIABLES
******************************************************************************/

/* All CAN Pin assignments are valid for: 
 *   STM32F745xx, STM32F746xx, STM32F750x8, STM32F756xx, 
 *   STM32F765xx, STM32F767xx, STM32F768Ax, STM32F769xx, 
 *   STM32F777xx, STM32F778Ax, STM32F779xx
 */
static PIN_ASSIGN Can2Pin_Rx[] = {
    { GPIOB, GPIO_PIN_5  },  /* #0: PB5  */ 
    { GPIOB, GPIO_PIN_12 }   /* #1: PB12 */ 
};
static PIN_ASSIGN Can2Pin_Tx[] = {
    { GPIOB, GPIO_PIN_6  },  /* #0: PB6  */
    { GPIOB, GPIO_PIN_13 }   /* #1: PB13 */
};

static BAUDRATE_TBL BaudrateTbl[] = {
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

static CAN_HandleTypeDef DrvCan2;

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

const CO_IF_CAN_DRV STM32F7xxCan2Driver = {
    DrvCanInit,
    DrvCanEnable,
    DrvCanRead,
    DrvCanSend,
    DrvCanReset,
    DrvCanClose
};

/******************************************************************************
* PUBLIC FUNCTIONS
******************************************************************************/

/* ST HAL CAN Receive Interrupt Handler */
void CAN2_RX0_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&DrvCan2);
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

    /* setup CAN RX and TX pins */
    gpio.Mode      = GPIO_MODE_AF_PP;
    gpio.Pull      = GPIO_NOPULL;
    gpio.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio.Alternate = GPIO_AF9_CAN2;
    gpio.Pin       = Can2Pin_Rx[CAN2_PIN_RX_SEL].Pin;
    HAL_GPIO_Init(Can2Pin_Rx[CAN2_PIN_RX_SEL].Port, &gpio);
    gpio.Pin       = Can2Pin_Tx[CAN2_PIN_TX_SEL].Pin;
    HAL_GPIO_Init(Can2Pin_Tx[CAN2_PIN_TX_SEL].Port, &gpio);

    /* CAN interrupt init */
    HAL_NVIC_SetPriority(CAN2_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN2_RX0_IRQn);
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
        while(1);    /* error not handled */
    }

    /* can controller mode */
    DrvCan2.Instance  = CAN2;
    DrvCan2.Init.Mode = CAN_MODE_NORMAL;

    /* baudrate settings */
    DrvCan2.Init.Prescaler     = BaudrateTbl[idx].Prescaler;
    DrvCan2.Init.SyncJumpWidth = BaudrateTbl[idx].SyncJumpWidth;
    DrvCan2.Init.TimeSeg1      = BaudrateTbl[idx].TimeSeg1;
    DrvCan2.Init.TimeSeg2      = BaudrateTbl[idx].TimeSeg2;

    /* feature select */
    DrvCan2.Init.TimeTriggeredMode    = DISABLE;
    DrvCan2.Init.AutoBusOff           = DISABLE;
    DrvCan2.Init.AutoWakeUp           = DISABLE;
    DrvCan2.Init.AutoRetransmission   = DISABLE;
    DrvCan2.Init.ReceiveFifoLocked    = DISABLE;
    DrvCan2.Init.TransmitFifoPriority = DISABLE;
    HAL_CAN_Init(&DrvCan2);
    HAL_CAN_Start(&DrvCan2);
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
    result = HAL_CAN_AddTxMessage(&DrvCan2, &frmHead, &frm->Data[0], &mailbox);
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

    err = HAL_CAN_GetRxMessage(&DrvCan2, CAN_RX_FIFO0, &frmHead, &frmData[0]);
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
    HAL_CAN_Init(&DrvCan2);
    HAL_CAN_Start(&DrvCan2);
}

static void DrvCanClose(void)
{
    HAL_CAN_Stop(&DrvCan2);
}
