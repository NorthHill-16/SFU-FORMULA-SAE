/** @file notification.c 
*   @brief User Notification Definition File
*   @date 08-Feb-2017
*   @version 04.06.01
*
*   This file  defines  empty  notification  routines to avoid
*   linker errors, Driver expects user to define the notification. 
*   The user needs to either remove this file and use their custom 
*   notification function or place their code sequence in this file 
*   between the provided USER CODE BEGIN and USER CODE END.
*
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


/* Include Files */

#include "esm.h"
#include "sys_selftest.h"
#include "adc.h"
#include "can.h"
#include "gio.h"
#include "sci.h"
#include "rti.h"
#include "sys_dma.h"

/* USER CODE BEGIN (0) */

#include "A_control_methods.h"
#include "A_exception_handler.h"
#include <stdio.h>



static uint8 connection_monitor_start_1 = 0;
static uint8 connection_monitor_start_2 = 0;


/* USER CODE END */
#pragma WEAK(esmGroup1Notification)
void esmGroup1Notification(uint32 channel)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (1) */
/* USER CODE END */
}

/* USER CODE BEGIN (2) */
/* USER CODE END */
#pragma WEAK(esmGroup2Notification)
void esmGroup2Notification(uint32 channel)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (3) */
/* USER CODE END */
}

/* USER CODE BEGIN (4) */
/* USER CODE END */
#pragma WEAK(memoryPort0TestFailNotification)
void memoryPort0TestFailNotification(uint32 groupSelect, uint32 dataSelect, uint32 address, uint32 data)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (5) */
/* USER CODE END */
}

/* USER CODE BEGIN (6) */
/* USER CODE END */
#pragma WEAK(memoryPort1TestFailNotification)
void memoryPort1TestFailNotification(uint32 groupSelect, uint32 dataSelect, uint32 address, uint32 data)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (7) */
/* USER CODE END */
}

/* USER CODE BEGIN (8) */


static uint8 ret_safety_check = 0;      //Return value for safety_check(), declares here reduce executions loop


/* USER CODE END */
#pragma WEAK(rtiNotification)
void rtiNotification(uint32 notification)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (9) */


    switch(notification){

        case 0x1:       //Run every 101 ms


            if( stat == RUNNING){

                speed =  (uint16) (  (float)( curr_gear * GEAR_STEP *  (uint16)adc_data[0].value )  / 4096);       //Convert 12 bit ADC scale to motor drive speed scale
                update_speed_canData();                                                                         //Update tx_data array
                canTransmit(canREG1, canMESSAGE_BOX1, tx_data);                                                 //Transmit speed to Motor driver

            }


            break;


        case 0x2:       //Run every 100 ms

            if(connection_monitor_start_1 == 1 && !check_enable_switch1_connectivity()){     //Firstly, check whether it is passing STANDBY mode, if it has not passed yet,
                                                                            //keep track of connectivity is useless. It needs to run "State Machine First"
                                                                            //If it has passed, check its connectivity

                setError(ERR_ENABLE_SWITCH1_DISCONNECT);
            }

            if(connection_monitor_start_2 == 1 && !check_enable_switch2_connectivity()){   //Same reason
                setError(ERR_ENABLE_SWITCH2_DISCONNECT);
            }

            adcUpdateData();                                                //Get data from battery voltage, ampere meter
            canUpdateData();                                                //Get data from Motor driver


            if( (ret_safety_check = safety_check()) != 0){                  //After updating data, check whether there are some security error
                                                                            //and warning.
                if(ret_safety_check <= 100){
                    setError(ret_safety_check);
                    return;                                                 //If error exists, return, since stat sets to SHUTDOWN, 0x1 and 0x2 stop executing.
                }else{
                    setWarnning(ret_safety_check);
                }

            }



        break;



    case 0x4:           //For Data Logging - Run every 350 ms


        //Send to PC by SCI2
        {
            /*
                v_battery
                v_cap
                a_battery
                v_throttle
                t_heatSink
                t_motor
                t_ptc
                a_ac_motor
                v_ac_motor
                torque_motor
                speed_vihecle
                current_battery_voltage
                current_battery_ampere
                speed
                curr_gear
                stat
                timer
             */

            uint8 buffer[38];

            buffer[0] = (v_battery.data & 0xFF00) >> 8U;
            buffer[1] = v_battery.data & 0xFF;
            buffer[2] = (v_cap.data & 0xFF00) >> 8U;
            buffer[3] = v_cap.data & 0xFF;
            buffer[4] = (a_battery.data & 0xFF00) >> 8U;
            buffer[5] = a_battery.data & 0xFF;
            buffer[6] = (v_throttle.data & 0xFF00) >> 8U;
            buffer[7] = v_throttle.data & 0xFF;
            buffer[8] = (t_heatSink.data & 0xFF00) >> 8U;
            buffer[9] = t_heatSink.data & 0xFF;
            buffer[10] = (t_motor.data & 0xFF00) >> 8U;
            buffer[11] = t_motor.data & 0xFF;
            buffer[12] = (t_ptc.data & 0xFF00) >> 8U;
            buffer[13] = t_ptc.data & 0xFF;
            buffer[14] = (a_ac_motor.data & 0xFF00) >> 8U;
            buffer[15] = a_ac_motor.data & 0xFF;
            buffer[16] = (v_ac_motor.data & 0xFF00) >> 8U;
            buffer[17] = v_ac_motor.data & 0xFF;
            buffer[18] = (torque_motor.data & 0xFF00) >> 8U;
            buffer[19] = torque_motor.data & 0xFF;
            buffer[20] = (speed_vihecle.data & 0xFF00) >> 8U;
            buffer[21] = speed_vihecle.data & 0xFF;
            buffer[22] = (current_battery_voltage & 0xFF00) >> 8U;
            buffer[23] = current_battery_voltage & 0xFF;
            buffer[24] = (current_battery_ampere & 0xFF00) >> 8U;
            buffer[25] = current_battery_ampere & 0xFF;
            buffer[26] = (speed & 0xFF00) >> 8U;
            buffer[27] = speed & 0xFF;

            buffer[28] = curr_gear;
            buffer[29] = stat;
            buffer[30] = (timer & 0xFF00000000000000) >> 56U;
            buffer[31] = (timer & 0x00FF000000000000) >> 48U;
            buffer[32] = (timer & 0x0000FF0000000000) >> 40U;
            buffer[33] = (timer & 0x000000FF00000000) >> 32U;
            buffer[34] = (timer & 0x00000000FF000000) >> 24U;
            buffer[35] = (timer & 0x0000000000FF0000) >> 16U;
            buffer[36] = (timer & 0x000000000000FF00) >> 8U;
            buffer[37] = (timer & 0x00000000000000FF) >> 0U;


            sciSend(scilinREG, 38, buffer);
        }

        break;




    case 0x8:           //Run every 100 ms

        ++timer;        //I put this here instead of putting in 0x1 or 0x2 section, since in here, it could
                        //relatively precise that 100 ms run once. And 0x1 and 0x2 run depends on status, here, always run.

        break;

    default :
        break;
    }














/* USER CODE END */
}

/* USER CODE BEGIN (10) */
/* USER CODE END */
#pragma WEAK(adcNotification)
void adcNotification(adcBASE_t *adc, uint32 group)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (11) */

/* USER CODE END */
}

/* USER CODE BEGIN (12) */
/* USER CODE END */
#pragma WEAK(canErrorNotification)
void canErrorNotification(canBASE_t *node, uint32 notification)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (13) */
/* USER CODE END */
}

#pragma WEAK(canStatusChangeNotification)
void canStatusChangeNotification(canBASE_t *node, uint32 notification)  
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (14) */






/* USER CODE END */
}

#pragma WEAK(canMessageNotification)
void canMessageNotification(canBASE_t *node, uint32 messageBox)  
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (15) */


    //I have not written receiving data handler here since we do not
    //read data as soon as it arrives. We read data every 400 ms in this
    //code, you can reduce the timer interval through HalCoGen


/* USER CODE END */
}

/* USER CODE BEGIN (16) */


#define LOCK_TIME 2                                                     //Example, if this is 2, it means lock for 200 ms. Means in 200ms, this code could not be run twice.
static uint64 locker = 0;                                               //Variable for locking, solve for debouncing.

uint8 isStandbySignalReceived(gioPORT_t *port, uint32 bit){
    return (port == gioPORTA && bit == 0 && gioGetBit(port, bit));      //Make sure it is GIOA[0], also need to make sure it is logical 1 on that pin
                                                                        //If not do this, when switch powers up first, the gioNotification() does not
                                                                        //trigger even pin is positive, when switch "open", the gioNotification() would
                                                                        //be trigger, which is a bug, this could solve it.
}
uint8 isModuleSignalReceived(gioPORT_t *port, uint32 bit){
    return (port == gioPORTA && bit == 1 && gioGetBit(port, bit));      //Make sure it is GIOA[1], also need to make sure it is logical 1 on that pin
}




/* USER CODE END */
#pragma WEAK(gioNotification)
void gioNotification(gioPORT_t *port, uint32 bit)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (19) */


    if( stat == UNLOCK && isStandbySignalReceived(port, bit) ){

        if ( lock(locker, LOCK_TIME) != 0 )   return;           //Two lines solving for debouncing, use lock mechanism.
        locker = timer;                                         //Update the locker


        connection_monitor_start_1 = 1;
        timer_delay(2);

        stat = STANDBY;

        gioDisableNotification(gioPORTA, 0);                    //Disable the notification, so that this function("gioNotification()") would not
                                                                //be trigger when GIOA[0] changes any more.

    }else if( stat == STANDBY && isModuleSignalReceived(port, bit) ){

        if ( lock(locker, LOCK_TIME) != 0 )   return;
        locker = timer;


        connection_monitor_start_2 = 1;
        timer_delay(2);

        stat = PRECHARGE;
        gioDisableNotification(gioPORTA, 1);


        connect_enable_switch1(1);                              //Connect the pre-charge switch(S_1) and ground(S_3)


        uint8 ret;                                              //Return value

        if( (ret = prechargeStateCheck()) != 0){                //0 is pass, smaller or equal to 100 is error, greater than 100 is warnning

            if(ret <= 100){
                setError(ret);
                return;
            }else{
                setWarnning(ret);
            }

        }



        timer_delay(2);





        stat = HIGH_VOLTAGE;
        connect_enable_switch2(1);                              //Connect the High_Voltage switch(S_2) and ground(S_3)
                                                                //Just remind that here I connect the ground again, see the function

        if( (ret = highVoltageStateCheck()) != 0){
            if(ret <= 100){
                setError(ret);
                return;
            }else{
                setWarnning(ret);
            }
        }


        timer_delay(2);

        stat = RUNNING;                                         //Running mode starts

    }






/* USER CODE END */
}

/* USER CODE BEGIN (20) */
/* USER CODE END */

#pragma WEAK(sciNotification)
void sciNotification(sciBASE_t *sci, uint32 flags)     
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (29) */
/* USER CODE END */
}

/* USER CODE BEGIN (30) */
/* USER CODE END */



/* USER CODE BEGIN (43) */
/* USER CODE END */


/* USER CODE BEGIN (47) */
/* USER CODE END */


/* USER CODE BEGIN (50) */
/* USER CODE END */


/* USER CODE BEGIN (53) */
/* USER CODE END */

#pragma WEAK(dmaGroupANotification)
void dmaGroupANotification(dmaInterrupt_t inttype, uint32 channel)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (54) */
/* USER CODE END */
}
/* USER CODE BEGIN (55) */
/* USER CODE END */

/* USER CODE BEGIN (56) */
/* USER CODE END */

/* USER CODE BEGIN (58) */
/* USER CODE END */

/* USER CODE BEGIN (60) */
/* USER CODE END */
