/*
 * receive_sensor_values.h
 *
 *  Created on: 28-Oct-2017
 *      Author: Admin
 */

#ifndef RECEIVE_SENSOR_VALUES_H_
#define RECEIVE_SENSOR_VALUES_H_
#ifdef __cplusplus
extern "C" {
#endif

//#include "periodic_scheduler/periodic_callback.h"

#include "_can_dbc/generated_Viserion.h"
#include "can.h"

//SENSORS_VALUES_t sensor_st;

//For Message Ids
enum{
    Sensor_Data_Id = 101,
    Motor_Speed_Id = 203,
};

//For Directions
enum{
    straight,
    full_left,
    slight_left,
    full_right,
    slight_right

};

//For Speed
enum{
    reverse,
    brake,
    slow,
    medium,
    fast
};


bool receiveSensorValues(unsigned int speed,unsigned int direction,can_msg_t *crx,dbc_msg_hdr_t *rx);
bool checkSensorValues(unsigned int speed,unsigned int direction);
#ifdef __cplusplus
}
#endif
#endif /* RECEIVE_SENSOR_VALUES_H_ */
/*
//For LD Indicators
{
    min_m = 5,
    l = 10,
    m = 20,
    r = 30,
    l_r = 40,
    m_l = 50,
    m_l_r = 60,
    m_r = 70,
    min_l_r = 80,
    no_obstacle = 90,
    reverse_brake = 1,
    reverse = 2
}
*/
