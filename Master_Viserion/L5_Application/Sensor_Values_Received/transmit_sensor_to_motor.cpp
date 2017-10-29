/*
 * transmit_sensor_to_motor.cpp
 *
 *  Created on: 29-Oct-2017
 *      Author: Admin
 */
#include "can.h"
#include "Sensor_Values_Received/transmit_sensor_to_motor.h"
#include "generated_Viserion.h"

bool transmit_speed(uint8_t speed)
{
    MASTER_SPEED_t speed_msg;
    speed_msg.MASTER_Maintain_Speed = speed;
    can_msg_t can_speed = {0};
    dbc_msg_hdr_t msg_hdr = dbc_encode_MASTER_SPEED(can_speed.data.bytes, &speed_msg);
    can_speed.msg_id = msg_hdr.mid;
    can_speed.frame_fields.data_len = msg_hdr.dlc;

    return CAN_tx(can1, &can_speed, 0);
}

bool transmit_direction(uint8_t direction)
{
    MASTER_DIRECTION_t direction_msg;
    direction_msg.MASTER_Maintain_Direction = direction;
    can_msg_t can_direction = {0};
    dbc_msg_hdr_t msg_hdr = dbc_encode_MASTER_DIRECTION(can_direction.data.bytes, &direction_msg);
    can_direction.msg_id = msg_hdr.mid;
    can_direction.frame_fields.data_len = msg_hdr.dlc;

    return CAN_tx(can1, &can_direction, 0);
}


