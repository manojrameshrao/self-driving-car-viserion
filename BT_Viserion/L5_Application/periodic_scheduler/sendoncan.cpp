#include "sendoncan.h"
#include "can.h"
#include "_can_dbc/generated_Viserion.h"
#include "dbc_app_send_can_msg.h"
#include "send_bluetooth_heartbeat.h"
#include "receive_heartbeats.h"
#include "io.hpp"
#include <stdio.h>

SEND_ROUTE_COORDINATES_t coordinate ={0};
GET_START_COORDINATES_t start_coordinate={0};
CHECKPOINTS_SENT_t send_complete={0};
BT_START_CMD_t car_Start={0};
BT_STOP_CMD_t car_Stop={0};

void sendStartCoordinate(){
	start_coordinate.GET_COORDINATES=1;
	if(dbc_encode_and_send_GET_START_COORDINATES(&start_coordinate))
		LE.on(1);
}

void sendEndOfCoordinares(){
    send_complete.BT_CHECKPOINTS_DONE=11;
               //send can msg to geo for coordinates transfer complete
    dbc_encode_and_send_CHECKPOINTS_SENT(&send_complete);
     LE.on(1);
}

void sendAllCordinates(double latti,double longi){

	coordinate.SET_LATTITUDE = latti;
	coordinate.SET_LONGITUDE = longi;
     printf("s /n");
	if(dbc_encode_and_send_SEND_ROUTE_COORDINATES(&coordinate))
		LE.toggle(2);
}


void sendCarStart(){
	car_Start.BT_READY=1;
	dbc_encode_and_send_BT_START_CMD(&car_Start);
}

void sendCarStop(){
	car_Stop.BT_ABORT=1;
	dbc_encode_and_send_BT_STOP_CMD(&car_Stop);

}
