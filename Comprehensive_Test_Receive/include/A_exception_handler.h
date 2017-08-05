

#ifndef _A_EXCEPTION_HANDLER
#define _A_EXCEPTION_HANDLER


#include "A_control_methods.h"



//Error--------------------------------------------------------

#define ERR_ENABLE_SWITCH_DISCONNECT       1U
#define ERR_ADC_PIN_LOST_CONNECTION         3U
#define ERR_STAT_MACHINE_LOST_CONNECTION    4U
#define ERR_CAN_MSG_RETRIEVE_OUT_OF_TIME    5U
#define ERR_ADC_MSG_NOT_RECEIVED            6U
#define ERR_CAN_MSG_MISS_TOO_MUCH            7U
#define ERR_ADC_RETRIEVE_DATA_OUT_OF_TIME   8U
#define ERR_GIO_CONNECTION_FAILED_TOO_MANY  9U




#define ERR_MICRO_CONTROLLER_INTERNAL_ERROR               99U


//Warning--------------------------------------------------------
#define WARN_MISS_MOTOR_MSG                     101U
#define WARN_MISS_ADC_DATA                      102U
#define WARN_MISS_CAN_DATA                      103U





#define WARN_UNKNOWN_ISSUE                  150U





void setError(uint8 val);
void setWarnning(uint8 val);












#endif

