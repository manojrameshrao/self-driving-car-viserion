/*
 * motor_init.h
 *
 *  Created on: 30-Oct-2017
 *      Author: Admin
 */

#ifndef MOTOR_INIT_H_
#define MOTOR_INIT_H_

bool motor_init(void);

enum{
    startMotor = 0,
    startBT = 1,
    startSensor = 2 ,
    startGeo = 3
};

#endif /* MOTOR_INIT_H_ */
