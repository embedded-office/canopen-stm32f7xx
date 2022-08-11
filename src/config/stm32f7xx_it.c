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
#include "stm32f7xx_it.h"
#include "clock_app.h"

/******************************************************************************
* PUBLIC FUNCTIONS
******************************************************************************/

/* handlers not used */
void NMI_Handler(void) {;}
void SVC_Handler(void) {;}
void DebugMon_Handler(void) {;}
void PendSV_Handler(void) {;}

/* errors not handled */
void HardFault_Handler(void) { while (1); }
void MemManage_Handler(void) { while (1); }
void BusFault_Handler(void) { while (1); }
void UsageFault_Handler(void) { while (1); }

/* systick is configured to 1kHz */
void SysTick_Handler(void)                  
{
    /* needed to get timouts in HAL  */
    HAL_IncTick();

    /* collect elapsed timed actions */
    COTmrService(&Clk.Tmr);                 
}

/* ST HAL CAN receive callback */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    /* process CAN frame with CANopen protocol */
    if (hcan->Instance == CAN1) {
        CONodeProcess(&Clk);
    };
}
