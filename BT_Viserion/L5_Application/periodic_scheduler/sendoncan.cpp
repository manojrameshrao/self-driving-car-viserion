/*
 * sendoncan.cpp
 *
 *  Created on: 27-Nov-2017
 *      Author: pratap
 */




#include "sendoncan.h"
#include "can.h"
#include "_can_dbc/generated_Viserion.h"
#include "dbc_app_send_can_msg.h"
#include "send_bluetooth_heartbeat.h"
#include "receive_heartbeats.h"
#include "io.hpp"
#include <stdio.h>

GET_ROUTE_COORDINATES_t coordinate ={0};
GET_START_COORDINATES_t start_coordinate={0};

void sendStartCoordinate(){
	start_coordinate.GET_COORDINATES=1;
	if(dbc_encode_and_send_GET_START_COORDINATES(&start_coordinate))
		LE.on(1);
}



void sendAllCordinates(double latti,double longi){

	coordinate.SET_LATTITUDE = latti; //recieve[size].get_lattitude();
	coordinate.SET_LONGITUDE = longi;  //recieve[size].get_longitude();
     printf("s /n");
	if(dbc_encode_and_send_GET_ROUTE_COORDINATES(&coordinate))
		LE.toggle(2);
}
