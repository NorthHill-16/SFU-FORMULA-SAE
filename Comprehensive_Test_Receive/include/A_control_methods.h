
#ifndef _A_CONTROL_METHODS

#define _A_CONTROL_METHODS

#include "can.h"
#include "gio.h"
#include "adc.h"
#include "A_exception_handler.h"


typedef enum _GEAR{  GEAR1 = 1, GEAR2, GEAR3  } Gear;
typedef enum _STATE {SHUTDOWN, LOCK, UNLOCK, STANDBY, PRECHARGE, HIGH_VOLTAGE, RUNNING } State;


#define TIME_SHIFT 10               //The shift value for variable "timer", it needs to shift for some reason
#define GEAR_STEP 420               //Each Gear's range
#define ADC_DATA_CONVERT_NUM 3      //The total number of ADC pin used
#define MAX_CAN_MSG_DELAY 20000000 * 1      //1 seconds


extern uint8 reg_connection_flags;

extern adcData_t adc_data[ADC_DATA_CONVERT_NUM];    //ADC converted result stores here
extern Gear curr_gear;                              //Gear variable
extern State stat;                                  //Current status


extern uint8 tx_data[8];                            //DCAN transmit data
extern uint8 rx_data[8];                            //DCAN receive data

extern uint64 timer;                                //Local timer, keep track of time


extern uint16 current_battery_voltage;              //From battery voltage meter
extern uint16 current_battery_ampere;               //From battery ampere meter


extern uint16 speed;                                //Speed of throttle generated


typedef struct data_receive_t{

    uint16 data;
    float32 scaling;
    uint16 offset;

} data_receive;


extern data_receive v_battery ;
extern data_receive v_cap ;
extern data_receive a_battery ;
extern data_receive v_throttle ;
extern data_receive t_heatSink ;
extern data_receive t_motor ;
extern data_receive t_ptc ;
extern data_receive a_ac_motor ;
extern data_receive v_ac_motor ;
extern data_receive torque_motor ;
extern data_receive speed_vihecle ;



extern uint8 connection_monitor_start_1;
extern uint8 connection_monitor_start_2;

extern uint8 check_enable_switch1_connectivity();
extern uint8 check_enable_switch2_connectivity();


uint8 pre_standbyStateCheck();
uint8 pre_prechargeStateCheck();
uint8 pre_highVoltageStateCheck();
uint8 pre_runningStateCheck();

uint8 post_standbyStateCheck();
uint8 post_prechargeStateCheck();
uint8 post_highVoltageStateCheck();
uint8 post_runningStateCheck();


void connect_enable_switch1(uint8 val);
void connect_enable_switch2(uint8 val);



void adcUpdateData(void);
void canUpdateData(void);
void msg_force_update();


void timer_delay(float val);
void loop_delay(uint64 val);



uint8 lock(uint64 lk, uint8 de);

#endif

