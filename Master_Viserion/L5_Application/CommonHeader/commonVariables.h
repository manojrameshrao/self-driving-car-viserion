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

//For Message Ids
enum{
    BT_Start = 83,
    BT_Stop = 84,
    Sensor_Data_Id = 101,
    Motor_Speed_Id = 203,
    Geo_Angles = 401,
    Geo_Dest_reached = 405
};


extern state_machine gCurrentState;
extern bool gChangeState;

//extern unsigned int gSpeed;
//extern unsigned int gDirection;
//extern unsigned int gGeoSpeed;
//extern unsigned int gGeoDirection;

#endif /* COMMONVARIABLES_H_ */
