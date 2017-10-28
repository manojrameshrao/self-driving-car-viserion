/*
 * receive_sensor_values.h
 *
 *  Created on: 28-Oct-2017
 *      Author: Admin
 */

#ifndef RECEIVE_SENSOR_VALUES_H_
#define RECEIVE_SENSOR_VALUES_H_

enum{
    sensor_Data_Id = 101,
    Motor_Speed_Id = 203
};

bool receiveSensorValues(void);
#endif /* RECEIVE_SENSOR_VALUES_H_ */
