/*
 * GPS.h
 *
 *  Created on: Oct 29, 2017
 *      Author: Ajinkya
 */

#ifndef L5_APPLICATION_PERIODIC_SCHEDULER_GPS_H_
#define L5_APPLICATION_PERIODIC_SCHEDULER_GPS_H_

#include "uart2.hpp"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "io.hpp"
#include "math.h"
#include "can.h"
#include "printf_lib.h"

struct cordinates{
     double latitude = 0;
     double longitude = 0;
};
extern Uart2& GPS_data;
extern cordinates current, projection, checkpoint;
extern char buffer[200];
/* variable to flag start when number of check points are received */
extern bool isNumberCheckPointsReceived;
extern uint8_t no_checkpoint_reached;
extern cordinates checkpoints_array[10];
#define TO_RAD (M_PI / 180)
#define LATITTUDE_START_INDEX (2)
#define LONGITUDE_START_INDEX (4)
#define WEST_COAST (-1)
#define EAST_COAST  (1)
//#define TEST_STATIC
//#define DEBUG


void init_GPS_module();
void get_GPS_data();
double convert_to_degrees(double value);
void set_projection_data(double latitude, double longitude);
void set_checkpoint_data(double latitude, double longitude);
float get_bearing_angle();
bool get_satallites_status(uint8_t satellite);
bool get_GNGGA_status(char * format);
bool checkpoint_reached();
bool destination_reached();
float to_radians(float value);
float get_bearing_angle_haversine();
float to_degrees(float value);
bool GPS_ready();
bool GPS_receive_data_processing(can_msg_t can_received_message);
void send_current_cordinates(bool flag);
void send_all_chekpoints_received(bool flag);
#endif /* L5_APPLICATION_PERIODIC_SCHEDULER_GPS_H_ */
