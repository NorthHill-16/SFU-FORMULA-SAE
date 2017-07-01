

#ifndef _A_EXCEPTION_HANDLER

#define _A_EXCEPTION_HANDLER


#include "A_control_methods.h"

#define ERR_ENABLE_SWITCH1_DISCONNECT       0x0000
#define ERR_ENABLE_SWITCH2_DISCONNECT       0x0001

#define ERR_ADC_PIN_LOST_CONNECTION         0x0002

#define ERR_STAT_MACHINE_LOST_CONNECTION    0x0003


void setError(uint8 val);
void setWarnning(uint8 val);












#endif

