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

#include "drv_nvm_i2c1_at24c256.h"
#include "stm32f7xx_hal.h"

/******************************************************************************
* PRIVATE TYPE DEFINITION
******************************************************************************/

typedef struct PIN_ASSIGN_T {
    GPIO_TypeDef *Port;
    uint16_t      Pin;
    uint8_t       Alternate;
} PIN_ASSIGN;

/******************************************************************************
* PRIVATE DEFINES
******************************************************************************/

#define EEPROM_ADDRESS     (uint16_t)0xA0 /* address of I2C device bit 1..7  */
#define EEPROM_TIMEOUT     (uint32_t)1000 /* timeout for each transfer: 1s   */
#define EEPROM_PAGE_SIZE   (uint16_t)64   /* EEPROM page size                */
#define EEPROM_ACK_POLLING 1000           /* retry on ack polling            */

/* default pin assignment: I2C_SCL -> PB8, I2C1_SDA -> PB9 */
#define I2C1_PIN_SCL_SEL     1
#define I2C1_PIN_SDA_SEL     1

/******************************************************************************
* PRIVATE VARIABLES
******************************************************************************/

static PIN_ASSIGN I2C1Pin_Scl[] = {
    { GPIOB, GPIO_PIN_6, GPIO_AF4_I2C1 },  /* #0: PB6 */ 
    { GPIOB, GPIO_PIN_8, GPIO_AF4_I2C1 }   /* #1: PB8 */ 
};
static PIN_ASSIGN I2C1Pin_Sda[] = {
    { GPIOB, GPIO_PIN_7, GPIO_AF4_I2C1 },  /* #0: PB7 */
    { GPIOB, GPIO_PIN_9, GPIO_AF4_I2C1 }   /* #2: PB9 */
};

static I2C_HandleTypeDef DrvI2CBus;

/******************************************************************************
* PRIVATE FUNCTIONS
******************************************************************************/

static void     DrvNvmInit  (void);
static uint32_t DrvNvmRead  (uint32_t start, uint8_t *buffer, uint32_t size);
static uint32_t DrvNvmWrite (uint32_t start, uint8_t *buffer, uint32_t size);

/******************************************************************************
* PUBLIC VARIABLE
******************************************************************************/

const CO_IF_NVM_DRV STM32F7xx_I2C1_AT24C256_NvmDriver = {
    DrvNvmInit,
    DrvNvmRead,
    DrvNvmWrite
};

/******************************************************************************
* PRIVATE FUNCTIONS
******************************************************************************/

static void DrvNvmInit(void)
{
    HAL_StatusTypeDef err;
    GPIO_InitTypeDef gpio = {0};
    RCC_PeriphCLKInitTypeDef pclk = {0};

    pclk.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
    pclk.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
    err = HAL_RCCEx_PeriphCLKConfig(&pclk);
    if (err != HAL_OK) {
        while(1);    /* error not handled */
    }

    /* Peripheral clocks enable */
    __HAL_RCC_I2C1_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* I2C1 GPIO Configuration: PB8 -> I2C1_SCL, PB9 -> I2C1_SDA */
    gpio.Mode = GPIO_MODE_AF_OD;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio.Alternate = I2C1Pin_Scl[I2C1_PIN_SCL_SEL].Alternate;
    gpio.Pin       = I2C1Pin_Scl[I2C1_PIN_SCL_SEL].Pin;
    HAL_GPIO_Init(I2C1Pin_Scl[I2C1_PIN_SCL_SEL].Port, &gpio);
    gpio.Alternate = I2C1Pin_Sda[I2C1_PIN_SDA_SEL].Alternate;
    gpio.Pin       = I2C1Pin_Sda[I2C1_PIN_SDA_SEL].Pin;
    HAL_GPIO_Init(I2C1Pin_Sda[I2C1_PIN_SDA_SEL].Port, &gpio);

    DrvI2CBus.Instance = I2C1;
    DrvI2CBus.Init.Timing = 0x00200922;
    DrvI2CBus.Init.OwnAddress1 = 0;
    DrvI2CBus.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    DrvI2CBus.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    DrvI2CBus.Init.OwnAddress2 = 0;
    DrvI2CBus.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    DrvI2CBus.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    DrvI2CBus.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    err = HAL_I2C_Init(&DrvI2CBus);
    if (err != HAL_OK) {
        while(1);    /* error not handled */
    }
    err = HAL_I2CEx_ConfigAnalogFilter(&DrvI2CBus, I2C_ANALOGFILTER_ENABLE);
    if (err != HAL_OK) {
        while(1);    /* error not handled */
    }
    err = HAL_I2CEx_ConfigDigitalFilter(&DrvI2CBus, 0);
    if (err != HAL_OK) {
        while(1);    /* error not handled */
    }
    HAL_I2CEx_EnableFastModePlus(I2C_FASTMODEPLUS_I2C1);
}

static uint32_t DrvNvmRead(uint32_t start, uint8_t *buffer, uint32_t size)
{
    HAL_StatusTypeDef err;
    uint32_t  result   = 0;
    uint16_t  memStart = (uint16_t)start;
    uint16_t  memSize  = (uint16_t)size;
    uint8_t  *memData  = buffer;

    while (memSize > EEPROM_PAGE_SIZE) {
        err = HAL_I2C_Mem_Read(
            &DrvI2CBus,
            EEPROM_ADDRESS,
            memStart,
            I2C_MEMADD_SIZE_16BIT,
            memData,
            EEPROM_PAGE_SIZE,
            EEPROM_TIMEOUT);
        if (err != HAL_OK) {
            return (result);
        }
        memStart += EEPROM_PAGE_SIZE;
        memSize  -= EEPROM_PAGE_SIZE;
        memData  += EEPROM_PAGE_SIZE;
        result   += EEPROM_PAGE_SIZE;
    }
    err = HAL_I2C_Mem_Read(
        &DrvI2CBus,
        EEPROM_ADDRESS,
        memStart,
        I2C_MEMADD_SIZE_16BIT,
        memData,
        memSize,
        EEPROM_TIMEOUT);
    if (err != HAL_OK) {
        return (result);
    }
    result += memSize;

    return (result);
}

static uint32_t DrvNvmWrite(uint32_t start, uint8_t *buffer, uint32_t size)
{
    HAL_StatusTypeDef err;
    uint32_t  result   = 0;
    uint16_t  memStart = (uint16_t)start;
    uint16_t  memSize  = (uint16_t)size;
    uint8_t  *memData  = buffer;
    uint16_t  memPart  = 0;

    /* write data up to the next block boundary */
    memPart = EEPROM_PAGE_SIZE - (memStart % EEPROM_PAGE_SIZE);
    if ((memPart < memSize) &&
        (memPart > 0      )) {
        err = HAL_I2C_Mem_Write(
            &DrvI2CBus,
            EEPROM_ADDRESS,
            memStart,
            I2C_MEMADD_SIZE_16BIT,
            memData,
            memPart,
            EEPROM_TIMEOUT);
        if (err != HAL_OK) {
            return (result);
        }
        memStart += memPart;
        memData  += memPart;
        memSize  -= memPart;
        result   += memPart;
        /* ACK polling during EEPROM internal write operations */
        err = HAL_I2C_IsDeviceReady(
            &DrvI2CBus,
            EEPROM_ADDRESS,
            EEPROM_ACK_POLLING,
            EEPROM_TIMEOUT);
        if (err != HAL_OK) {
            return (result);
        }
    }

    /* write all full blocks */
    while (memSize > EEPROM_PAGE_SIZE) {
        err = HAL_I2C_Mem_Write(
            &DrvI2CBus,
            EEPROM_ADDRESS,
            memStart,
            I2C_MEMADD_SIZE_16BIT,
            memData,
            EEPROM_PAGE_SIZE,
            EEPROM_TIMEOUT);
        if (err != HAL_OK) {
            return (result);
        }
        memStart += EEPROM_PAGE_SIZE;
        memData  += EEPROM_PAGE_SIZE;
        memSize  -= EEPROM_PAGE_SIZE;
        result   += EEPROM_PAGE_SIZE;
        /* ACK polling during EEPROM internal write operations */
        err = HAL_I2C_IsDeviceReady(
            &DrvI2CBus,
            EEPROM_ADDRESS,
            EEPROM_ACK_POLLING,
            EEPROM_TIMEOUT);
        if (err != HAL_OK) {
            return (result);
        }
    }

    /* write the last partly filled block */
    if (memSize > 0) {
        err = HAL_I2C_Mem_Write(
            &DrvI2CBus,
            EEPROM_ADDRESS,
            memStart,
            I2C_MEMADD_SIZE_16BIT,
            memData,
            memSize,
            EEPROM_TIMEOUT);
        if (err != HAL_OK) {
            return (result);
        }
        result += memSize;
        /* ACK polling during EEPROM internal write operations */
        err = HAL_I2C_IsDeviceReady(
            &DrvI2CBus,
            EEPROM_ADDRESS,
            EEPROM_ACK_POLLING,
            EEPROM_TIMEOUT);
        if (err != HAL_OK) {
            return (result);
        }
    }
    return (result);
}
