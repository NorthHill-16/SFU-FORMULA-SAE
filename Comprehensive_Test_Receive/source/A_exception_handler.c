

#include "A_exception_handler.h"


void setError(uint8 val){

    connect_enable_switch1(0);
    connect_enable_switch2(0);
    stat = SHUTDOWN;

    speed = 0;

    //canTransmit(canREG1, canMESSAGE_BOX5, &val);



}
void setWarnning(uint8 val){


}
