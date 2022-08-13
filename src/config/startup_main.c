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

#include "stm32f7xx_hal.h"
#include "clock_app.h"

/******************************************************************************
* PRIVATE FUNCTIONS
******************************************************************************/

static void SystemClock_Config(void);

/******************************************************************************
* PUBLIC FUNCTIONS
******************************************************************************/

void _init(void) {;}         /* stub function, called in __libc_init_array() */

HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)    /* Disable 1ms Tick */
{
    /* We can disable the regular 1ms time tick, because we use our timer
     * driver to setup timer-actions with an accuracy of 1us.
     *
     * Unless we use one of the following HAL modules, the tick is unused:
     * - hal_dsi, hal_eth, hal_mmc, hal_sd or ll_usb
     */
    (void)TickPriority;
    return HAL_OK;
}

void HAL_MspInit(void)            /* ST HAL lowlevel initialization callback */
{
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_RCC_SYSCFG_CLK_ENABLE();
}

int main(void)                    /* main entry point for controller startup */
{
    HAL_Init();
    SystemClock_Config();

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();

    AppMain();                  /* ok, we are ready to start the application */
}

/******************************************************************************
* PRIVATE FUNCTIONS
******************************************************************************/

static void SystemClock_Config(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;
    HAL_StatusTypeDef ret = HAL_OK;
    
    /* Enable Power Control clock */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);  

    /* Enable HSE Oscillator and activate PLL with HSE as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 25;
    RCC_OscInitStruct.PLL.PLLN = 432;  
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    RCC_OscInitStruct.PLL.PLLR = 2;  
    
    ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
    if (ret != HAL_OK) {
        while(1);             /* error not handled */
    }
    
    /* Activate the OverDrive to reach the 216 MHz Frequency */  
    ret = HAL_PWREx_EnableOverDrive();
    if (ret != HAL_OK) {
        while(1);             /* error not handled */
    }
    
    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; /* SYSCLK    : 216 MHz */
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;        /* FCLK, HCLK: 216 MHz */
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;         /* APB1:  54 MHz, APB1-TMR: 108 MHz */
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;         /* APB2: 108 MHz, APB2-TMR: 216 MHz */

    ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
    if (ret != HAL_OK) {
        while(1);             /* error not handled */
    }
}
