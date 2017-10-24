#include "send_motor_heartbeat.h"
#include "_can_dbc/generated_Viserion.h"


void send_motor_heartbeat(void)
{
	HB_MOTORS_t motor_msg={1};
    motor_msg.MOTOR_ALIVE=1;
	dbc_encode_and_send_HB_MOTORS(&motor_msg);
}
