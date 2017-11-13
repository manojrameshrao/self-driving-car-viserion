/*
 * receive_sensor_values.h
 *
 *  Created on: 28-Oct-2017
 *      Author: Admin
 */

#ifndef RECEIVE_SENSOR_VALUES_H_
#define RECEIVE_SENSOR_VALUES_H_

//#include "_can_dbc/generated_Viserion.h"
enum{
    sensor_Data_Id = 101,
    Motor_Speed_Id = 203,
    sensor_Hb_Id = 95
};

bool receiveSensorValues(void);
bool checkSensorValues(void);

#endif /* RECEIVE_SENSOR_VALUES_H_ */
