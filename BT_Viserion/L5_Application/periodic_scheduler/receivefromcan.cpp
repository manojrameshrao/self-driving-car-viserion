/*
 * receivefromcan.cpp
 *
 *  Created on: 27-Nov-2017
 *      Author: pratap
 */


#include "receivefromcan.h"
#include "can.h"
#include "_can_dbc/generated_Viserion.h"
#include <iostream>
using namespace std;

SEND_START_COORDINATES_t send_start_co={0};
SEND_COMPASS_HEAD_t send_compass_dir={0};
void receiveallcanmsges(Uart3 & u3){
	 can_msg_t can_msg;

	    while (CAN_rx(can1, &can_msg, 0) && u3.getRxQueueSize()==0)
	    {

	    	cout<<"check"<<endl;
	        dbc_msg_hdr_t can_msg_hdr;
	        can_msg_hdr.dlc = can_msg.frame_fields.data_len;
	        can_msg_hdr.mid = can_msg.msg_id;

	        switch(can_msg_hdr.mid){
	        case 405:{

	        	if(dbc_decode_SEND_START_COORDINATES(&send_start_co,can_msg.data.bytes,&can_msg_hdr)){
	        		//write function for what to do when u receive coordinates from gps
	        	}

	        }
	        break;
	        case 401 :{
	        	if(dbc_decode_SEND_COMPASS_HEAD(&send_compass_dir,can_msg.data.bytes,&can_msg_hdr)){
	        		        		//write function for what to do when u receive degreeval from compass
	        		        	}
	        }
	        break;
	        }

	        }

}
