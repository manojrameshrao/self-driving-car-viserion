

#include "receivefromcan.h"
#include "can.h"
#include "_can_dbc/generated_Viserion.h"
#include <iostream>
#include "bluetoothtransmission.h"
#include "display.hpp"
#include <stdio.h>
#include <math.h>
#include    <stdlib.h>
#include "io.hpp"


#include <string.h>
using namespace std;

SEND_COORDINATES_t send_start_co={0};
SEND_CURRENT_COORDINATES_t current_start_co={0};
SEND_GEO_ANGLES_t send_compass_dir={0};
MOTOR_STATUS_t speed ={0};
SENSORS_VALUES_t sensor_values={0};
ALL_CHECKPOINTS_RECEIVED_t check ={0};
SEND_GEO_READY_t lock ={0};
DESTINATION_REACHED_t reached ={0};

double slatt=0;
double slongi=0;

double send_latt=0;
double send_longi=0;

int sensor_left=0;
int sensor_right=0;
int sensor_middle=0;
int sensor_rear=0;

int count =0;
int head=0;
int bear=0;
int car_speed=0;

int head_d =0;
int angle_diff =0;

void receiveallcanmsges(Uart3 & u3){
	can_msg_t can_msg;

	while (CAN_rx(can1, &can_msg, 0) && u3.getRxQueueSize()==0)
	{

		//cout<<"check"<<endl;
		dbc_msg_hdr_t can_msg_hdr;
		can_msg_hdr.dlc = can_msg.frame_fields.data_len;
		can_msg_hdr.mid = can_msg.msg_id;

		switch(can_msg_hdr.mid)
		{

		case 101:{
			if(dbc_decode_SENSORS_VALUES(&sensor_values,can_msg.data.bytes,&can_msg_hdr))
			{
				sensor_left = sensor_values.SENSOR_left_in;
				sensor_right=sensor_values.SENSOR_right_in;
				sensor_middle = sensor_values.SENSOR_middle_in;
				sensor_rear = sensor_values.SENSOR_back_in;


			}

			break;
		}

		case 400:
		{

			if(dbc_decode_SEND_COORDINATES(&send_start_co,can_msg.data.bytes,&can_msg_hdr))
			{
				char slatti[9];
				char slongii[11];
				char ssend[22]="L";

				slatt=send_start_co.SEND_LATTITUDE;
				slongi=send_start_co.SEND_LONGITUDE;


				sprintf(slatti,"%f",slatt);
				sprintf(slongii,"%f",slongi);

				strcat(ssend,slatti);
				strcat(ssend,",");
				strcat(ssend,slongii);
				u3.putline(ssend,portMAX_DELAY);
				//cout<<send_start_co.SEND_LATTITUDE<<" "<<send_start_co.SEND_LONGITUDE<<endl;
				//write function for what to do when u receive coordinates from gps
			}

		}
		break;

		case 407:
		{

			if(dbc_decode_SEND_CURRENT_COORDINATES(&current_start_co,can_msg.data.bytes,&can_msg_hdr))
			{
				send_latt=current_start_co.SEND_CURRENT_LATTITUDE;
				send_longi=current_start_co.SEND_CURRENT_LONGITUDE;

				//write function for what to do when u receive coordinates from gps
			}

		}
		break;

		case 401 :{
			if(dbc_decode_SEND_GEO_ANGLES(&send_compass_dir,can_msg.data.bytes,&can_msg_hdr))
			head=send_compass_dir.SEND_HEAD;
			bear=send_compass_dir.SEND_BEAR;
			setDIRECTION_INIT(head);
			setSENSOR_LEFT_INIT(bear);


			if(head>180)
			{
				head_d = head -360;
			}
			else
			{
				head_d=head;
			}
			angle_diff = bear-head_d;
			setBATTERY_INIT(angle_diff);
			//setBATTERY_INIT(bear);
		}
		break;

		case 406: {
			if(dbc_decode_SEND_GEO_READY(&lock,can_msg.data.bytes,&can_msg_hdr))
			{
				char not_lock[3]="NO";
				u3.putline(not_lock);
			}

		}break;

		case 404: if(dbc_decode_ALL_CHECKPOINTS_RECEIVED(&check,can_msg.data.bytes,&can_msg_hdr))
		{
			if(check.GEO_CHECKPOINTS_READY == 0)
			{
				LE.toggle(4);
			}
		}
		break;

		case 405: if(dbc_decode_DESTINATION_REACHED(&reached,can_msg.data.bytes,&can_msg_hdr))
		{
			sendtoapp("R",u3);
		}
		break;
		case 301 :{
			if(dbc_decode_MOTOR_STATUS(&speed,can_msg.data.bytes,&can_msg_hdr))

			{
				car_speed=speed.MOTOR_Send_Speed;
				setSPEED_INIT(car_speed);

				//write function for what to do when u receive degreeval from compass
			}
		}
		break;
		}

	}

}
void sendDetails(Uart3 & u3,bool stats){
	if(stats == 1){

		char sensor_send[15]="O";
		char value_left[3];
		char value_right[3];
		char value_middle[3];
		char value_back[3];

		itoa(sensor_left,value_left,10);
		itoa(sensor_right,value_right,10);
		itoa(sensor_middle,value_middle,10);
		itoa(sensor_rear,value_back,10);

		strcat(sensor_send,value_right);
		strcat(sensor_send,",");
		strcat(sensor_send,value_middle);
		strcat(sensor_send,",");
		strcat(sensor_send,value_left);
		strcat(sensor_send,",");
		strcat(sensor_send,value_back);
		sendtoapp(sensor_send,u3);


		char compass[4]="D";
		char value[3];
		itoa(head,value,10);
		strcat(compass,value);
		strcat(compass,"\n");
		sendtoapp(compass,u3);


		char mph[4]="S";
		char value1[3];
		itoa(car_speed,value1,10);
		strcat(mph,value1);
		strcat(mph,"\n");
		sendtoapp(mph,u3);

		char bearing[4]="B";
		char value2[3];
		itoa(bear,value2,10);
		strcat(bearing,value2);
		strcat(bearing,"\n");
		sendtoapp(bearing,u3);


		char send_latti[9];
		char send_longii[11];
		char send_curr[22]="C";

		sprintf(send_latti,"%f",send_latt);
		sprintf(send_longii,"%f",send_longi);

		strcat(send_curr,send_latti);
		strcat(send_curr,",");
		strcat(send_curr,send_longii);
		u3.putline(send_curr,portMAX_DELAY);





	}


}
