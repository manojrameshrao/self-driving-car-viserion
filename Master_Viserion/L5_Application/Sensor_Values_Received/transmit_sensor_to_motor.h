/*
 * transmit_sensor_to_motor.h
 *
 *  Created on: 29-Oct-2017
 *      Author: Admin
 */

#ifndef TRANSMIT_SENSOR_TO_MOTOR_H_
#define TRANSMIT_SENSOR_TO_MOTOR_H_

#include"stdint.h"

bool transmit_speed(uint8_t speed);
bool transmit_direction(uint8_t direction);

#endif /* TRANSMIT_SENSOR_TO_MOTOR_H_ */
