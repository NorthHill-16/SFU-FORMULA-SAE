


#include "A_control_methods.h"


adcData_t adc_data[ADC_DATA_CONVERT_NUM] = {0};

data_receive v_battery =        {.data = 0, .scaling = 0.0625,      .offset = 0};
data_receive v_cap =            {.data = 0, .scaling = 0.0625,      .offset = 0};
data_receive a_battery =        {.data = 0, .scaling = 0.0625,      .offset = 0};
data_receive v_throttle =       {.data = 0, .scaling = 0.00390625,  .offset = 0};
data_receive t_heatSink =       {.data = 0, .scaling = 0,           .offset = 0};
data_receive t_motor =          {.data = 0, .scaling = 0.00390625,  .offset = 0};
data_receive t_ptc =            {.data = 0, .scaling = 1.0,         .offset = 0};
data_receive a_ac_motor =       {.data = 0, .scaling = 1.0,         .offset = 0};
data_receive v_ac_motor =       {.data = 0, .scaling = 0.0625,      .offset = 0};
data_receive torque_motor =     {.data = 0, .scaling = 0.0625,      .offset = 0};
data_receive speed_vihecle =    {.data = 0, .scaling = 0.0625,      .offset = 0};

Gear curr_gear = GEAR1;
State stat = UNLOCK;
uint8 tx_data[8] = {0};
uint8 rx_data[8] = {0};
uint64 timer = TIME_SHIFT;
uint16 current_battery_voltage = 0;
uint16 current_battery_ampere = 0;
uint16 speed = 0;




uint8 prechargeStateCheck(){

    return 0;
}



uint8 highVoltageStateCheck(){

    return 0;
}


uint8 safety_check(){

    return 0;
}







uint8 check_enable_switch1_connectivity(){
    return gioGetBit(gioPORTA, 0);
}
uint8 check_enable_switch2_connectivity(){
    return gioGetBit(gioPORTA, 1);
}

void connect_enable_switch1(uint8 val){
    gioSetBit(gioPORTA, 5, val);  //close S_1 switch
    gioSetBit(gioPORTA, 7, val);  //close S_3 switch
}


void connect_enable_switch2(uint8 val){
    gioSetBit(gioPORTA, 6, val);  //close S_2 switch
    gioSetBit(gioPORTA, 7, val);  //close S_1 switch
}


void adcUpdateData(void){


    adcStartConversion(adcREG1, adcGROUP1);
    while( !adcIsConversionComplete(adcREG1, adcGROUP1) );

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

        }

        if( canGetData(canREG1, canMESSAGE_BOX3, rx_data) != 0 ){

            t_heatSink.data = (uint16)rx_data[0];
            t_motor.data = (uint16)( (uint16)rx_data[1] | ((uint16)rx_data[2] << 8U)  );
            t_ptc.data = (uint16)( (uint16)rx_data[3] | ((uint16)rx_data[4] << 8U)  );

        }

        if( canGetData(canREG1, canMESSAGE_BOX4, rx_data) != 0 ){

            a_ac_motor.data = (uint16)( (uint16)rx_data[0] | ((uint16)rx_data[1] << 8U)  );
            v_ac_motor.data = (uint16)( (uint16)rx_data[2] | ((uint16)rx_data[3] << 8U)  );
            torque_motor.data = (uint16)( (uint16)rx_data[4] | ((uint16)rx_data[5] << 8U)  );
            speed_vihecle.data = (uint16)( (uint16)rx_data[6] | ((uint16)rx_data[7] << 8U)  );
        }




}

void canNewMessageReceived(){

    uint64 tic = timer;
    while( !canIsRxMessageArrived(canREG1, canMESSAGE_BOX2) ){
        if( loop_runOutOfTime(tic, 50) == 1 )  return;
    }
    tic = timer;
    while( !canIsRxMessageArrived(canREG1, canMESSAGE_BOX3) ){
        if( loop_runOutOfTime(tic, 50) == 1 )  return;
    }
    tic = timer;
    while( !canIsRxMessageArrived(canREG1, canMESSAGE_BOX4) ){
        if( loop_runOutOfTime(tic, 50) == 1 )  return;
    }
}


void update_speed_canData(void){

    tx_data[0] = (uint8)(speed & 0xFF);
    tx_data[1] = (uint8)(speed >> 8U);
}

uint8 lock(uint64 seed, uint8 delay){
    return (timer - seed < delay) ? 1 : 0;
}


uint8 loop_runOutOfTime(uint64 seed, uint8 time){
    return (timer - seed < time) ? 0 : 1;
}


void timer_delay(float val){
    loop_delay( (uint32)(10000000 * val) );
}

void loop_delay(uint32 val){
    for(;val > 0; --val);
}
