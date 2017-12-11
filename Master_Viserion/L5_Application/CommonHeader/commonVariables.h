/*
 * fileCommon.h
 *
 *  Created on: 10-Dec-2017
 *      Author: Admin
 */

#ifndef COMMONVARIABLES_H_
#define COMMONVARIABLES_H_


typedef enum
{
    init_car,
    send_start,
    get_distance_heading,
    get_sensor_vals,
    stop_car,
    abort_mission,
    gps_no_lock
}state_machine;

extern state_machine gCurrentState;
extern bool gChangeState;

//extern unsigned int gSpeed;
//extern unsigned int gDirection;
//extern unsigned int gGeoSpeed;
//extern unsigned int gGeoDirection;

#endif /* COMMONVARIABLES_H_ */
