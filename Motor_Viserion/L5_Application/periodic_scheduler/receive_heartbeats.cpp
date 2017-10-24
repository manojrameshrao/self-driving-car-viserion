#include "receive_heartbeats.h"
#include "_can_dbc/generated_Viserion.h"
#include "io.hpp"
#include "can.h"


const uint32_t                             HB_BT__MIA_MS = 3000;
const HB_BT_t                              HB_BT__MIA_MSG = {0};
const uint32_t                             HB_SENSORS__MIA_MS= 3000;
const HB_SENSORS_t                         HB_SENSORS__MIA_MSG={0};
const uint32_t                             HB_GEO__MIA_MS= 3000;
const HB_GEO_t                             HB_GEO__MIA_MSG={0};
const uint32_t                             HB_MASTER__MIA_MS= 3000;
const HB_MASTER_t                          HB_MASTER__MIA_MSG={0};


HB_SENSORS_t sensor_msg={0};
HB_GEO_t geo_msg = { 0 };
HB_BT_t bt_msg = { 0 };
HB_MASTER_t master_msg = { 0 };


void receive_heartbeats(void)
{
	 /* Turn all LEDs off every 100ms just to be able to see when they are turning on */
	    LE.off(1);
	    LE.off(2);
	    LE.off(3);
	    LE.off(4);

	    can_msg_t can_msg;

	    // Empty all of the queued, and received messages within the last 10ms (100Hz callback frequency)
	while(CAN_rx(can1,&can_msg,0))
		{
			dbc_msg_hdr_t can_msg_hdr;
		    can_msg_hdr.dlc = can_msg.frame_fields.data_len;
			can_msg_hdr.mid = can_msg.msg_id;

			switch(can_msg.msg_id)
			        {
			            case 95:dbc_decode_HB_SENSORS(&sensor_msg, can_msg.data.bytes, &can_msg_hdr);
						    LE.on(3);
						    break;
			            case 97:
			                dbc_decode_HB_GEO(&geo_msg, can_msg.data.bytes, &can_msg_hdr);
			                LE.on(2);
			                break;
			            case 98:
			                dbc_decode_HB_BT(&bt_msg, can_msg.data.bytes, &can_msg_hdr);
			                LE.on(3);
			                break;
			            case 99:
			                dbc_decode_HB_MASTER(&master_msg, can_msg.data.bytes, &can_msg_hdr);
			                LE.on(4);
			                break;
			        }
		}
	    if(dbc_handle_mia_HB_SENSORS(&sensor_msg, 100))
			LD.setNumber(1);
	    if(dbc_handle_mia_HB_BT(&bt_msg, 100))
	        LD.setNumber(2);
		if(dbc_handle_mia_HB_GEO(&geo_msg, 100))
			LD.setNumber(3);
		if(dbc_handle_mia_HB_MASTER(&master_msg, 100))
			LD.setNumber(4);
}



