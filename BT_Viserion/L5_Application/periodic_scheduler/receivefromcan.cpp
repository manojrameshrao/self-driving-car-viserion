

#include "receivefromcan.h"
#include "can.h"
#include "_can_dbc/generated_Viserion.h"
#include <iostream>
#include "bluetoothtransmission.h"


#include <string.h>
using namespace std;

SEND_START_COORDINATES_t send_start_co={0};
SEND_COMPASS_HEAD_t send_compass_dir={0};
MOTOR_STATUS_t speed ={0};
SENSORS_VALUES_t sensor_values={0};

int count =0;
void receiveallcanmsges(Uart3 & u3,bool stats){
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




			}

			break;
		}

		case 405:{

			if(dbc_decode_SEND_START_COORDINATES(&send_start_co,can_msg.data.bytes,&can_msg_hdr))
			{
				//write function for what to do when u receive coordinates from gps
			}

		}
		break;
		case 401 :{
			count++;
			if(count % 11)
				if(dbc_decode_SEND_COMPASS_HEAD(&send_compass_dir,can_msg.data.bytes,&can_msg_hdr) && stats == 1)

				{
					char compass[4]="D";
					int comp=send_compass_dir.SEND_HEAD;
					char value[3];
					itoa(comp,value,10);
					strcat(compass,value);
					sendtoapp(compass,u3);

					//write function for what to do when u receive degreeval from compass
				}
		}
		break;
		case 301 :{
			count++;
			if(count % 13)
				if(dbc_decode_MOTOR_STATUS(&speed,can_msg.data.bytes,&can_msg_hdr) && stats == 1)

				{
					char mph[4]="S";
					int car_speed=speed.MOTOR_Send_Speed;
					char value[3];
					itoa(car_speed,value,10);
					strcat(mph,value);
					sendtoapp(mph,u3);

					//write function for what to do when u receive degreeval from compass
				}
		}
		break;
		}

	}

}
