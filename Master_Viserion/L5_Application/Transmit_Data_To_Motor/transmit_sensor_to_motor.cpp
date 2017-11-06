/*
 * transmit_sensor_to_motor.cpp
 *
 *  Created on: 29-Oct-2017
 *      Author: Admin
 */
#include "can.h"
#include "Transmit_Data_To_Motor/transmit_sensor_to_motor.h"
#include "generated_Viserion.h"

bool transmit_to_motor(uint8_t speed, uint8_t direction )
{
    MASTER_SPEED_t motor_msg;
    motor_msg.MASTER_Maintain_Speed = speed;
    motor_msg.MASTER_Maintain_Direction = direction;
    can_msg_t can_motor = {0};
    dbc_msg_hdr_t msg_hdr = dbc_encode_MASTER_SPEED(can_motor.data.bytes, &motor_msg);
    can_motor.msg_id = msg_hdr.mid;
    can_motor.frame_fields.data_len = msg_hdr.dlc;

    return CAN_tx(can1, &can_motor, 0);
}



