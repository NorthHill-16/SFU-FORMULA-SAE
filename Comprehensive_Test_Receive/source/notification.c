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


/*  User Included Files  */

#include "A_control_methods.h"
#include "A_exception_handler.h"
//#include <stdio.h>      //Debug use



/*  Constants Define    */

#define ADC_MAX_DELAY_ITERATIONS    60  //Tolerance of how many CPU iterations ADC must retrieve data back. It cost 19 or 20 iterations to complete conversion in normal case
                                        //Range : 0 - 255

#define CAN_MAX_MSG_LOSS            9   //Tolerance of how many pieces of DCAN data could be lost
                                        //Range : 0 - 255



/*  Global Variable Declaration     */


uint8 reg_CTL = 0x00;                   //Bit 0:        Value == 1 -> State machine has been initialized before
                                        //              Value == 0 -> State machine has not been initialized yet
                                        //Bit 1:        Value == 1 -> RTI starts to monitor the connectivity for MD_EN, if disconnect, shutdown
                                        //              Value == 0 -> RTI does not monitor the connectivity for MD_EN
                                        //Bit 2:        Value == 1 -> RTI starts to monitor the connectivity for MD_RUN, if disconnect, shutdown
                                        //              Value == 0 -> RTI does not monitor the connectivity for MD_RUN
                                        //Bit [3:7]:    Reserved


static uint8 adc_max_delay_iterations;  //The variable holds remaining iterations for ADC retrieve data, initial value is ADC_MAX_DELAY_ITERATIONS
static uint8 can_msg_miss = 0;          //The variable holds missing DCAN MSG, initial value is 0, up to CAN_MAX_MSG_LOSS


/*  Function Signature Declaration     */

uint8 check_enable_switch1_connectivity();
uint8 check_enable_switch2_connectivity();

uint8 running_safety_check();
uint8 regular_safety_check();

void adcUpdateData(void);
void canUpdateData(void);
static void loop_delay_internal(uint32 val);


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
/* USER CODE END */
#pragma WEAK(rtiNotification)
void rtiNotification(uint32 notification)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (9) */


    switch(notification){

        case 0x1:       //Executing every 101 ms



            if( (reg_CTL & 0x02) == 0x02 && !check_enable_switch1_connectivity()){

                setError(ERR_ENABLE_SWITCH_DISCONNECT);
                return;
            }

            if( (reg_CTL & 0x04) == 0x04 && !check_enable_switch2_connectivity() ){
                setError(ERR_ENABLE_SWITCH_DISCONNECT);
                return;
            }


            adcStartConversion(adcREG1, adcGROUP1);
            adcUpdateData();
            //canUpdateData();



            if( stat == RUNNING){

                if( running_safety_check() != 0){                               //After updating data, check whether there are some security error
                                                                                //Warning return 0, error return 1
                    return;
                }


                speed =  (uint16) (  (float)( curr_gear * GEAR_STEP *  (uint32)adc_data[0].value )  / 4096);       //Convert 12 bit ADC scale to motor drive speed scale

                tx_data[0] = (uint8)(speed & 0xFF);
                tx_data[1] = (uint8)(speed >> 8U);

                canTransmit(canREG1, canMESSAGE_BOX1, tx_data);                                                 //Transmit speed to Motor driver

            } else {

                if(regular_safety_check() != 0){
                    return;
                }

             }

            break;


        case 0x2:       //  Reserved

        break;



        case 0x4:           //Reserved for formal version


        {
            /* Send data to PC by SCI2
             * Data Sequence Order :

                v_battery,  v_cap,  a_battery,  v_throttle, t_heatSink, t_motor
                t_ptc,  a_ac_motor, v_ac_motor, torque_motor,   speed_vihecle,  current_battery_voltage
                current_battery_ampere, speed,  curr_gear,  stat,   timer

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


    case 0x8:           //Executing every 100 ms

        ++timer;

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
/* USER CODE END */
}

/* USER CODE BEGIN (16) */
#include <stdio.h>

/* USER CODE END */
#pragma WEAK(gioNotification)
void gioNotification(gioPORT_t *port, uint32 bit)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (19) */


    loop_delay_internal(50);


    if( gioGetBit(port, bit) == 1 ){


        if( stat == SHUTDOWN && (reg_CTL & 0x01) == 1 ) {

            if( check_enable_switch1_connectivity() && check_enable_switch2_connectivity() && port == gioPORTA && bit == 1 ){

                goto lab_restart;

            }


        } else if ( (stat == UNLOCK || stat == SHUTDOWN ) && port == gioPORTA && bit == 0 && gioGetBit(gioPORTA, 0) == 1 && gioGetBit(gioPORTA, 1) == 0  ){

            reg_CTL |= 0x02;
            timer_delay(2);

            //msg_force_update();
            if( pre_standbyStateCheck() != 0 ){         //NOTE : pre_standbyStateCheck() function is responsible to trigger shutdown process
                return;
            }

            stat = STANDBY;

            //msg_force_update();
            if( post_standbyStateCheck() != 0 ){
                return;
            }


        } else if( stat == STANDBY && port == gioPORTA && bit == 1 && gioGetBit(gioPORTA, 0) == 1 && gioGetBit(gioPORTA, 1) == 1 ) {

            reg_CTL |= 0x04;

            lab_restart: timer_delay(2);            //Start point from SHUTDOWN mode back to RUNNING mode

            //msg_force_update();
            if( pre_prechargeStateCheck() != 0 ){
                return;
            }

            stat = PRECHARGE;
            connect_enable_switch1(1);                              //Connect switch S_1 and S_3

            //msg_force_update();
            if( post_prechargeStateCheck() != 0 ){
                return;
            }

            timer_delay(2);

            //msg_force_update();
            if( pre_highVoltageStateCheck() != 0 ){
                return;
            }

            stat = HIGH_VOLTAGE;
            connect_enable_switch2(1);                              //Connect switch S_2 and S_3

            //msg_force_update();
            if( post_highVoltageStateCheck() != 0 ){
                return;
            }

            timer_delay(2);

            //msg_force_update();
            if( pre_runningStateCheck() != 0 ){
                return;
            }

            stat = RUNNING;                                         //Running mode starts

            //msg_force_update();
            if( post_runningStateCheck() != 0 ){
                return;
            }

            reg_CTL |= 0x01;                //Indicating initialization has been done for once

        }

    }else{

        if( (reg_CTL & 0x06) != 0 ){

            setError(ERR_ENABLE_SWITCH_DISCONNECT);
            return;
        }

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




uint8 check_enable_switch1_connectivity(){
    return gioGetBit(gioPORTA, 0);
}
uint8 check_enable_switch2_connectivity(){
    return gioGetBit(gioPORTA, 1);
}


uint8 running_safety_check(){

    return 0;
}

uint8 regular_safety_check(){


    return 0;
}





void adcUpdateData(void){

    adc_max_delay_iterations = (uint8)ADC_MAX_DELAY_ITERATIONS;


    while(   adcIsConversionComplete(adcREG1,adcGROUP1) == 0 ){

        if( !(--adc_max_delay_iterations) ){
            setError(ERR_ADC_RETRIEVE_DATA_OUT_OF_TIME);
            return;
        }

    }


    if( adcGetData(adcREG1, 1U, adc_data) != ADC_DATA_CONVERT_NUM){
        setError(ERR_ADC_PIN_LOST_CONNECTION);
        return;
    }

    //Need Formula Here
    current_battery_voltage = adc_data[1].value;
    current_battery_ampere = adc_data[2].value;


}


void canUpdateData(void){


        if( canGetData(canREG1, canMESSAGE_BOX2, rx_data) != 0 ){

            v_battery.data = (uint16)( (uint16)rx_data[0] | ((uint16)rx_data[1] << 8U)  );
            v_cap.data = (uint16)( (uint16)rx_data[2] | ((uint16)rx_data[3] << 8U)  );
            a_battery.data = (uint16)( (uint16)rx_data[4] | ((uint16)rx_data[5] << 8U)  );
            v_throttle.data = (uint16)( (uint16)rx_data[6] | ((uint16)rx_data[7] << 8U)  );

        }else{
            ++can_msg_miss;
            setWarnning(WARN_MISS_MOTOR_MSG);
        }

        if( canGetData(canREG1, canMESSAGE_BOX3, rx_data) != 0 ){

            t_heatSink.data = (uint16)rx_data[0];
            t_motor.data = (uint16)( (uint16)rx_data[1] | ((uint16)rx_data[2] << 8U)  );
            t_ptc.data = (uint16)( (uint16)rx_data[3] | ((uint16)rx_data[4] << 8U)  );

        }else{
            ++can_msg_miss;
            setWarnning(WARN_MISS_MOTOR_MSG);
        }

        if( canGetData(canREG1, canMESSAGE_BOX4, rx_data) != 0 ){

            a_ac_motor.data = (uint16)( (uint16)rx_data[0] | ((uint16)rx_data[1] << 8U)  );
            v_ac_motor.data = (uint16)( (uint16)rx_data[2] | ((uint16)rx_data[3] << 8U)  );
            torque_motor.data = (uint16)( (uint16)rx_data[4] | ((uint16)rx_data[5] << 8U)  );
            speed_vihecle.data = (uint16)( (uint16)rx_data[6] | ((uint16)rx_data[7] << 8U)  );
        }else{
            ++can_msg_miss;
            setWarnning(WARN_MISS_MOTOR_MSG);
        }

        if(can_msg_miss >= CAN_MAX_MSG_LOSS){
            setError(ERR_CAN_MSG_MISS_TOO_MUCH);
            return;
        }


}




static void loop_delay_internal(uint32 val){
    for(val += 10 ;val > 10; --val);
}




/* USER CODE END */
