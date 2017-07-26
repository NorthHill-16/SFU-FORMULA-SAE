


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


uint8 pre_standbyStateCheck(){

    return !(check_enable_switch1_connectivity() ) ;
}
uint8 pre_prechargeStateCheck(){

    return !(check_enable_switch1_connectivity() );
}
uint8 pre_highVoltageStateCheck(){

    return !(check_enable_switch1_connectivity() & check_enable_switch2_connectivity());
}

uint8 pre_runningStateCheck(){

    return !(check_enable_switch1_connectivity() & check_enable_switch2_connectivity());
}
uint8 post_standbyStateCheck(){

    return !(check_enable_switch1_connectivity() );
}
uint8 post_prechargeStateCheck(){

    return !(check_enable_switch1_connectivity() & check_enable_switch2_connectivity());
}
uint8 post_highVoltageStateCheck(){

    return !(check_enable_switch1_connectivity() & check_enable_switch2_connectivity());
}
uint8 post_runningStateCheck(){

    return !(check_enable_switch1_connectivity() & check_enable_switch2_connectivity());
}







void connect_enable_switch1(uint8 val){
    gioSetBit(gioPORTA, 7, val);  //S_3 switch
    gioSetBit(gioPORTA, 5, val);  //S_1 switch

}


void connect_enable_switch2(uint8 val){
    gioSetBit(gioPORTA, 7, val);  //S_3 switch
    gioSetBit(gioPORTA, 6, val);  //S_2 switch
}








void msg_force_update(){

    int iterations = MAX_CAN_MSG_DELAY;
    while( !canIsRxMessageArrived(canREG1, canMESSAGE_BOX2) ){

        if( !(--iterations) ){
            setError(ERR_CAN_MSG_RETRIEVE_OUT_OF_TIME);
            return;
        }

    }

    if( canGetData(canREG1, canMESSAGE_BOX2, rx_data) != 0 ){

        v_battery.data = (uint16)( (uint16)rx_data[0] | ((uint16)rx_data[1] << 8U)  );
        v_cap.data = (uint16)( (uint16)rx_data[2] | ((uint16)rx_data[3] << 8U)  );
        a_battery.data = (uint16)( (uint16)rx_data[4] | ((uint16)rx_data[5] << 8U)  );
        v_throttle.data = (uint16)( (uint16)rx_data[6] | ((uint16)rx_data[7] << 8U)  );

    }else{
        setError(ERR_MICRO_CONTROLLER_INTERNAL_ERROR);
        return;
    }

    iterations = MAX_CAN_MSG_DELAY;
    while( !canIsRxMessageArrived(canREG1, canMESSAGE_BOX3) ) {

        if( !(--iterations) ){
            setError(ERR_CAN_MSG_RETRIEVE_OUT_OF_TIME);
            return;
        }

    }

    if( canGetData(canREG1, canMESSAGE_BOX3, rx_data) != 0 ){

        t_heatSink.data = (uint16)rx_data[0];
        t_motor.data = (uint16)( (uint16)rx_data[1] | ((uint16)rx_data[2] << 8U)  );
        t_ptc.data = (uint16)( (uint16)rx_data[3] | ((uint16)rx_data[4] << 8U)  );

    }else{
        setError(ERR_MICRO_CONTROLLER_INTERNAL_ERROR);
        return;
    }





    iterations = MAX_CAN_MSG_DELAY;
    while( !canIsRxMessageArrived(canREG1, canMESSAGE_BOX4) ) {

        if( !(--iterations) ){
            setError(ERR_CAN_MSG_RETRIEVE_OUT_OF_TIME);
            return;
        }

    }


    if( canGetData(canREG1, canMESSAGE_BOX4, rx_data) != 0 ){

        a_ac_motor.data = (uint16)( (uint16)rx_data[0] | ((uint16)rx_data[1] << 8U)  );
        v_ac_motor.data = (uint16)( (uint16)rx_data[2] | ((uint16)rx_data[3] << 8U)  );
        torque_motor.data = (uint16)( (uint16)rx_data[4] | ((uint16)rx_data[5] << 8U)  );
        speed_vihecle.data = (uint16)( (uint16)rx_data[6] | ((uint16)rx_data[7] << 8U)  );
    }else{
        setError(ERR_MICRO_CONTROLLER_INTERNAL_ERROR);
        return;
    }



    int max_delay = 60;

    while(  !adcIsConversionComplete(adcREG1,adcGROUP1) ){

        if( --max_delay ){
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


uint8 lock(uint64 seed, uint8 delay){
    return (timer - seed < delay) ? 1 : 0;
}



void timer_delay(float val){
    loop_delay( (uint64)(10000000 * val) );
}

void loop_delay(uint64 val){
    for(;val > 0; --val);
}
