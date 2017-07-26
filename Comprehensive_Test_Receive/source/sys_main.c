/** @file sys_main.c 
*   @brief Application main file
*   @date 08-Feb-2017
*   @version 04.06.01
*
*   This file contains an empty main function,
*   which can be used for the application.
*/

/* 
* Copyright (C) 2009-2016 Texas Instruments Incorporated - www.ti.com 
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/


/* USER CODE BEGIN (0) */


#include "can.h"
#include "gio.h"
#include "adc.h"
#include "rti.h"
#include "system.h"
#include "sys_core.h"
#include "esm.h"
#include "sys_vim.h"

//Debug Use
#include "sci.h"




/* USER CODE END */

/* Include Files */

#include "sys_common.h"

/* USER CODE BEGIN (1) */
/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */


//Function Declaration
void init();



/* USER CODE END */

int main(void)
{
/* USER CODE BEGIN (3) */


    init();
    while(1);


/* USER CODE END */

}


/* USER CODE BEGIN (4) */



void init(){


    _enable_interrupt_();
    _enable_IRQ();
    _enable_FIQ();


    //init CAN
    canInit();

    //init ADC
    adcInit();
    //adcEnableNotification(adcREG1, adcGROUP1);

    //init SCI2
    sciInit();

    //init RTI
    rtiInit();

    rtiEnableNotification(rtiNOTIFICATION_COMPARE0);
    rtiEnableNotification(rtiNOTIFICATION_COMPARE3);

    rtiEnableNotification(rtiNOTIFICATION_COMPARE2);        //Debug Use


    rtiStartCounter(rtiCOUNTER_BLOCK0);         // For Compare 0, Compare 4
    rtiStartCounter(rtiCOUNTER_BLOCK1);         // For Compare 2, COmpare 3

    //init GIO
    gioInit();
    gioEnableNotification(gioPORTA, 0);
    gioEnableNotification(gioPORTA, 1);

    //Some other configurations for gio is done in gio.c



    //your further initialization goes here ...


}









/* USER CODE END */
