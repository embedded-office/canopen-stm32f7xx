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
* \file     bsp_board.h
*
* \brief    Implementation of the Configuration Class BSP_BOARD
*
* <table class="mdTable" cellpadding="5" cellspacing="0" border="0"><tr><td>
* <h2> Perforce Depot: </h2>
*     $File: //stream_bsp/_root/platform/hw_stm32f769/env_gcc/demo/bsp_quickstart/cfg/bsp_board.h $
* $DateTime: 2016/09/14 17:15:51 $
* $Revision: #1 $
*   $Change: 84526 $
* </td></tr></table>
*
****************************************************************************************************
*/
/*----------------------------------------END OF HEADER-------------------------------------------*/

#ifndef _BSP_BOARD_H_
#define _BSP_BOARD_H_

/*
****************************************************************************************************
*                                             INCLUDES
****************************************************************************************************
*/

#include "bsp.h"

/*
****************************************************************************************************
*                                        EXTERNAL VARIABLES
****************************************************************************************************
*/

extern struct BSP_BOARD_T HW;

/*
****************************************************************************************************
*                                           DEFINITIONS
****************************************************************************************************
*/

#define HW_ERR       ((BSP_ERR*)&(HW.Err))
#define HW_CLK       ((BSP_CLK*)&(HW.Clk))
#define HW_EXC       ((BSP_EXC*)&(HW.Exc))

#define HW_SER       ((BSP_SER*)&(HW.Ser[0]))
#define HW_PIN       ((BSP_PIN*)&(HW.Pin))
#define HW_UART(n)   ((BSP_UART*)&(HW.Ser[n]))
#define HW_LED(n)    ((BSP_PIN*)&(HW.Led[n]))
#define HW_TMR(n)    ((BSP_TMR*)&(HW.Tmr[n]))

#define HW_TMR_MAX   2u
#define HW_MAX_LED   4u
#define HW_MAX_SER   1u

#define BSP_TEST_TICKTIMER  BSP_TMR_SYSTICK
#define HW_TICKTMR          HW_TMR(0)
#define BSP_TEST_TESTTIMER  BSP_TMR_TIM2
#define HW_TESTTMR          HW_TMR(1)
#define BSP_TEST_PIN        BSP_PIN_C00
#define HW_TEST_LED         HW_PIN
#define HW_TESTIRQ          BSP_IRQ_RTC_WKUP

/* workaround for ugly inconsistency */
#define HW_LED_MAX          HW_MAX_LED
#define TEST_LED_PIN        BSP_TEST_PIN



/*
****************************************************************************************************
*                                         TYPE DEFINITIONS
****************************************************************************************************
*/

typedef struct BSP_BOARD_T
{
    BSP_ERR Err;                                     /* - error management                        */
    BSP_CLK Clk;                                     /* - clock management                        */
    BSP_EXC Exc;                                     /* - exception handling                      */
    
                                                     /* used peripherals (see BSP Manual) */
    BSP_SER Ser[HW_MAX_SER];
    BSP_PIN Pin;
    BSP_PIN Led[HW_MAX_LED];
    BSP_TMR Tmr[HW_TMR_MAX];

} BSP_BOARD;

/*
****************************************************************************************************
*                                        FUNCTION PROTOTYPES
****************************************************************************************************
*/

void BSPLowInit (void);
void BSPInit    (void);


#endif                                               /* ifndef _BSP_BOARD_H_                      */
