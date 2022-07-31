/*
****************************************************************************************************
* (c) copyright by
*     Embedded Office GmbH & Co. KG       Tel : (07522) 97 00 08-0
*     August-Braun-Str. 1                 Fax : (07522) 97 00 08-99
*     D-88239 Wangen                      Mail: info@embedded-office.de
*                                         Web : http://www.embedded-office.de
*
* All rights reserved. Confidential and Proprietary. Protected by international copyright laws.
* Knowledge of the source code may not be used to write a similar product.
* This file may only be used in accordance with a license and should not be
* redistributed in any way.
****************************************************************************************************
*/
/*!
****************************************************************************************************
* \file     bsp_cfg.c
*
* \brief    Target Specific Configuration Tables
*
* <table class="mdTable" cellpadding="5" cellspacing="0" border="0"><tr><td>
* <h2> Perforce Depot: </h2>
*     $File: //stream_bsp/_root/platform/hw_stm32f769/env_gcc/demo/bsp_quickstart/cfg/bsp_cfg.c $
* $DateTime: 2016/09/14 17:15:51 $
* $Revision: #1 $
*   $Change: 84526 $
* </td></tr></table>
*
****************************************************************************************************
*/
/*----------------------------------------END OF HEADER-------------------------------------------*/

/*
****************************************************************************************************
*                                             INCLUDES
****************************************************************************************************
*/

#include "bsp_board.h"

/*
****************************************************************************************************
*                                         CONFIGURATIONS
****************************************************************************************************
*/

const BSP_CLK_CFG BSPClkCfg = {
    25000000L,                                        /* HSE_FREQ   = 25.0 MHz                    */
    32768L,                                           /* LSE_FREQ   = 32.768 kHz                  */
    1L,                                               /* HSE_ON     = HSE                         */
    0L,                                               /* HCLKDIV    = AHB Clock  = SYSCLK         */
    1L,                                               /* PCLK2DIV   = APB2 Clock = HCLK / 2       */
    2L,                                               /* PCLK1DIV   = APB1 Clock = HCLK / 4       */
    1L,                                               /* PLL_SRC    = HSE                         */
    25L,                                              /* PLLM       = 25                          */
    432,                                              /* PLLN       = 432                         */
    2L,                                               /* PLLP       = 2                           */
    9L,                                               /* PLLQ       = 9                           */
    2L,                                               /* SYSCLK_SRC = PLL                         */
    7L                                                /* LATENCY    = 7 waitstates                */
};

const BSP_EXC_CFG BSPExcCfg[] = {                    /* <used exception>         = <prio>         */
    BSP_EXC_CFG_END
};

const BSP_IRQ_CFG BSPIrqCfg[] = {                    /* <used interrupt>         = <prio>         */
    { BSP_IRQ_TIM2, 0},
    { BSP_IRQ_TIM3, 0},
    { BSP_IRQ_TIM4, 0},
    { BSP_IRQ_TIM5, 0},
    { BSP_IRQ_TIM6_DAC, 0},
    { BSP_IRQ_TIM7, 0},
    BSP_IRQ_CFG_END
};

/* LED0 and LED1 */
const BSP_PIN_CFG BSPPinCfg[] = {
    {BSP_PIN_C00,
     BSP_PIN_CFG_MODE_OUT,
     BSP_PIN_CFG_SPD_HIGH,
     BSP_PIN_CFG_OUT_PP,
     BSP_PIN_CFG_PUPD_NP},
    {BSP_PIN_I15,
     BSP_PIN_CFG_MODE_OUT,
     BSP_PIN_CFG_SPD_HIGH,
     BSP_PIN_CFG_OUT_PP,
     BSP_PIN_CFG_PUPD_PU},
    {BSP_PIN_J00,
     BSP_PIN_CFG_MODE_OUT,
     BSP_PIN_CFG_SPD_HIGH,
     BSP_PIN_CFG_OUT_PP,
     BSP_PIN_CFG_PUPD_PU},
    {BSP_PIN_J01,
     BSP_PIN_CFG_MODE_OUT,
     BSP_PIN_CFG_SPD_HIGH,
     BSP_PIN_CFG_OUT_PP,
     BSP_PIN_CFG_PUPD_PU},
    {BSP_PIN_J03,
     BSP_PIN_CFG_MODE_OUT,
     BSP_PIN_CFG_SPD_HIGH,
     BSP_PIN_CFG_OUT_PP,
     BSP_PIN_CFG_PUPD_PU},
    BSP_PIN_CFG_END
};

const BSP_SER_CFG BSPSerCfg[] = {
    {BSP_SER_USART1, 115200u, 0},                     /* Baud=115200                              */
    BSP_SER_CFG_END
};
