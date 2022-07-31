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
* \file     bsp_board.c
*
* \brief    Implementation of the Configuration Class BSP_BOARD
*
* <table class="mdTable" cellpadding="5" cellspacing="0" border="0"><tr><td>
* <h2> Perforce Depot: </h2>
*     $File: //stream_bsp/_root/platform/hw_stm32f769/env_gcc/demo/bsp_quickstart/cfg/bsp_board.c $
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
*                                          GLOBAL VARIABLES
****************************************************************************************************
*/

BSP_BOARD HW;

/*
****************************************************************************************************
*                                             FUNCIONS
****************************************************************************************************
*/

void BSPLowInit(void)
{
    BSPErrInit  (HW_ERR, 0L);                         /* initialize error management              */
    BSPClkInit  (HW_CLK);                             /* enable clock with configured PLL         */
    BSPExcInit  (HW_EXC);                             /* Initialize Exception Handling            */
    BSPIrqReset ();                                   /* reset interrupt management for safety    */
}

void BSPInit(void)
{
    BSPSerInit(HW_SER, BSP_SER_USART1);
    BSPTmrInit(HW_TICKTMR, BSP_TEST_TICKTIMER);
    BSPTmrInit(HW_TESTTMR, BSP_TEST_TESTTIMER);
    BSPPinInit(HW_LED(0), BSP_PIN_I15);
    BSPPinInit(HW_LED(1), BSP_PIN_J00);
    BSPPinInit(HW_LED(2), BSP_PIN_J01);
    BSPPinInit(HW_LED(3), BSP_PIN_J03);
}
