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

typedef struct PIN_ASSIGN_T {
    GPIO_TypeDef *Port;
    uint16_t      Pin; 
} PIN_ASSIGN;

/******************************************************************************
* PRIVATE VARIABLES
******************************************************************************/

static CAN_HandleTypeDef DrvCan;

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

/* CAN1 Pin assignments are valid for: 
 *   STM32F722xx, STM32F723xx, STM32F730x8, STM32F732xx, STM32F733xx, STM32F745xx,
 *   STM32F746xx, STM32F750x8, STM32F756xx, STM32F765xx, STM32F767xx, STM32F768Ax,
 *   STM32F769xx, STM32F777xx, STM32F778Ax, STM32F779xx
 * 
 *   PH14 not for: STM32F750x8, STM32F745xx, STM32F746xx
 */
PIN_ASSIGN CanPin_Rx[] = {
    { GPIOA, GPIO_PIN_11 },  /* #0: PA11 */ 
    { GPIOB, GPIO_PIN_8  },  /* #1: PB8  */ 
    { GPIOD, GPIO_PIN_0  },  /* #2: PD0  */ 
    { GPIOH, GPIO_PIN_14 },  /* #3: PH14 */
    { GPIOI, GPIO_PIN_9  }   /* #4: PI9  */
};
PIN_ASSIGN CanPin_Tx[] = {
    { GPIOA, GPIO_PIN_12 },  /* #0: PA12 */
    { GPIOB, GPIO_PIN_9  },  /* #2: PB9  */
    { GPIOD, GPIO_PIN_1  },  /* #3: PD1  */
    { GPIOH, GPIO_PIN_13 }   /* #4: PH13 */
};

/* default: CAN_RX -> PB8, CAN_TX -> PB9 */
#define CAN_PIN_RX_SEL  1
#define CAN_PIN_TX_SEL  1

/******************************************************************************
* PUBLIC FUNCTIONS
******************************************************************************/

/* ST HAL CAN Receive Interrupt Handler */
void CAN1_RX0_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&DrvCan);
}

/******************************************************************************
* PRIVATE FUNCTIONS
******************************************************************************/

static void DrvCanInit(void)
{
    GPIO_InitTypeDef gpio = {0};

    /* Peripheral clocks enable (for simplicity, enable all possible ports) */
    __HAL_RCC_CAN1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOI_CLK_ENABLE();

    /* setup CAN RX and TX pins */
    gpio.Mode      = GPIO_MODE_AF_PP;
    gpio.Pull      = GPIO_NOPULL;
    gpio.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio.Alternate = GPIO_AF9_CAN1;
    gpio.Pin       = CanPin_Rx[CAN_PIN_RX_SEL].Pin;
    HAL_GPIO_Init(CanPin_Rx[CAN_PIN_RX_SEL].Port, &gpio);
    gpio.Pin       = CanPin_Tx[CAN_PIN_TX_SEL].Pin;
    HAL_GPIO_Init(CanPin_Tx[CAN_PIN_TX_SEL].Port, &gpio);

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
    DrvCan.Instance  = CAN1;
    DrvCan.Init.Mode = CAN_MODE_NORMAL;

    /* baudrate settings */
    DrvCan.Init.Prescaler     = BaudrateTbl[idx].Prescaler;
    DrvCan.Init.SyncJumpWidth = BaudrateTbl[idx].SyncJumpWidth;
    DrvCan.Init.TimeSeg1      = BaudrateTbl[idx].TimeSeg1;
    DrvCan.Init.TimeSeg2      = BaudrateTbl[idx].TimeSeg2;

    /* feature select */
    DrvCan.Init.TimeTriggeredMode    = DISABLE;
    DrvCan.Init.AutoBusOff           = DISABLE;
    DrvCan.Init.AutoWakeUp           = DISABLE;
    DrvCan.Init.AutoRetransmission   = DISABLE;
    DrvCan.Init.ReceiveFifoLocked    = DISABLE;
    DrvCan.Init.TransmitFifoPriority = DISABLE;
    HAL_CAN_Init(&DrvCan);
    HAL_CAN_Start(&DrvCan);
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
    result = HAL_CAN_AddTxMessage(&DrvCan, &frmHead, &frm->Data[0], &mailbox);
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

    err = HAL_CAN_GetRxMessage(&DrvCan, CAN_RX_FIFO0, &frmHead, &frmData[0]);
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
    HAL_CAN_Init(&DrvCan);
    HAL_CAN_Start(&DrvCan);
}

static void DrvCanClose(void)
{
    HAL_CAN_Stop(&DrvCan);
}
