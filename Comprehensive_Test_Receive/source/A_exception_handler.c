

#include "A_exception_handler.h"
#include "A_control_methods.h"

void setError(uint8 val){

    connect_enable_switch2(0);
    stat = SHUTDOWN;
    connect_enable_switch1(0);


    speed = 0;

    canTransmit(canREG1, canMESSAGE_BOX5, &val);


    gioSetBit(gioPORTA, 2, 1);      //Error LED


}
void setWarnning(uint8 val){

    canTransmit(canREG1, canMESSAGE_BOX5, &val);

}
