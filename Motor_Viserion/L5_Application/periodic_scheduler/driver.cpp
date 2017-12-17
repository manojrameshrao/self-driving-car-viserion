#include "driver.hpp"
#include "can.h"
#include "gpio.hpp"
#include "io.hpp"
#include "eint.h"
#include "lpc_pwm.hpp"
#include "dbc_app_send_can_msg.h"
#include "_can_dbc/generated_Viserion.h"
#include <stdint.h>
#include "motor.hpp"
#include "send_motor_heartbeat.h"

const float MOTOR_INIT = 18;
#define FORWARD_SLOW        19
#define FORWARD_MEDIUM      19.2    //19.2
#define FORWARD_FAST        20.5
#define REVERSE             16.1
#define BRAKES              17
#define SERVO_CENTER        17.7
#define SERVO_HARD_LEFT     10.9
#define SERVO_SLIGHT_LEFT   14.9
#define SERVO_HARD_RIGHT    25
#define SERVO_SLIGHT_RIGHT  20.7
#define GEAR_RATIO 			12.58			//Gear to Wheel Ratio: Wheel rotates 1x for every 12.58 revolution of the DC motor
#define PI					3.14159
#define WHEEL_RADIUS		2.794			//Unit is centimeters
#define CONST_KPH           0.036
#define STOP_SPEED          1
#define SLOW_SPEED          7 //8
#define MEDIUM_SPEED        10 //8

bool IGNORE_ORIENT=false;
//bool START_OP=false;
//bool STOP_OP=false;

motor car;
MASTER_SPEED_t speed = { 0 };
MOTOR_STATUS_t sending_speed_kph = { 1 };
MASTER_START_CMD_t start_car = { 0 };
MASTER_STOP_CMD_t stop_car = { 0 };

extern const uint32_t MASTER_SPEED__MIA_MS = 10000;
extern const MASTER_SPEED_t MASTER_SPEED__MIA_MSG = { 1, 0 };

PWM motor_speed(PWM::pwm1, 10);
PWM motor_dir(PWM::pwm2, 10);

orientation_t orient = invalid;

float value, kph = 0;
int speed_req = 0;
int count_speed = 0;
int rps = 0;
int stop_flag = 0;
float compare_pwm = 0;
volatile uint8_t speed_calc = 1;

uint32_t curr_rps = 0;
uint32_t req_rps = 0;

//Default values
static float motor_speed_val = MOTOR_INIT, servo_dir_val = 18, req_servo_dir_val=18;
static int reverse_count = 0;
GPIO rpm_pin(P2_6);
int16_t tilt_x, tilt_y;
bool reverse_flag = false;
bool motor_init_flag = false;
bool brake_flag = false;

//CAN Message struct
can_msg_t motor_message;
dbc_msg_hdr_t can_msg_hdr;

void bus_off(uint32_t)
{
    CAN_reset_bus(can1);
}

void data_ovr(uint32_t)
{
    CAN_reset_bus(can1);
}

void count_rpm()
{
    car.incrRevolutions();
}

void initializeCAN(){
    //Configure CAN1
    CAN_init(can1, 100, 100, 100, bus_off, data_ovr);
    CAN_bypass_filter_accept_all_msgs();
    CAN_reset_bus(can1);

    //Set GPIO as input and enable interrupt for pin connected to RPM sensor
    rpm_pin.setAsInput();
    eint3_enable_port2(6, eint_falling_edge, count_rpm);

}
void initializeCAR()
{
/*
    //Configure CAN1
    CAN_init(can1, 100, 100, 100, bus_off, data_ovr);
    CAN_bypass_filter_accept_all_msgs();
    CAN_reset_bus(can1);
*/

    //Initialize motor and servo
    car.setmotorspeed(MOTOR_INIT);
    motor_speed.set(car.getmotorspeed());
    car.setmotordir(18);
    motor_dir.set(car.getmotordir());
/*
    //Set GPIO as input and enable interrupt for pin connected to RPM sensor
    rpm_pin.setAsInput();
    eint3_enable_port2(6, eint_falling_edge, count_rpm);
*/
}

bool calculate_speed(int no_of_rev)
{
    kph = (2 * PI * WHEEL_RADIUS * CONST_KPH * no_of_rev);
    car.setkph(kph);
    //LD.setNumber(kph);
    curr_rps = car.getRevolutions();


        sending_speed_kph.MOTOR_Send_Speed = curr_rps;
        dbc_encode_and_send_MOTOR_STATUS(&sending_speed_kph);
        send_motor_heartbeat();

    return true;

    //make kph local and can retrun the calculated speed
}

void reverse_init()
{
#if 1
    //if(reverse_flag){
    if (reverse_count == 5) {
        LE.on(1);
        motor_speed.set(MOTOR_INIT); //motor_speed_val = MOTOR_INIT;
        LD.setNumber(11);
    }
    else if (reverse_count == 10)
    {
        LD.setNumber(22);
        motor_speed.set(REVERSE);
        LE.off(1);
    }
    else if(reverse_count==15) {
        LD.setNumber(33);
        motor_speed.set(MOTOR_INIT);
    }
    else if(reverse_count>20) {
        LD.setNumber(44);
        motor_speed.set(REVERSE);
        reverse_flag=false;
        motor_speed_val=18.9;
    }
    reverse_count++;
    //}
#endif
#if 0
    if (reverse_count == 5)
    {
        LE.on(1);
        motor_speed.set(MOTOR_INIT); //motor_speed_val = MOTOR_INIT;
        LD.setNumber(44);
        //reverse_count=8;
    }
    else if (reverse_count == 10)
    {
        LD.setNumber(22);
        motor_speed.set(REVERSE);
        reverse_count=0;
        LE.off(1);
    }
    reverse_count++;
#endif
}

int orienttype()
{
    return orient;
}

uint32_t getcurrrps()
{
    return curr_rps;
}

int getreqrps()
{
    return req_rps;
}

float feedback()
{
    //pass orientation, req_rps and curr_rps as parameters
    //int required_rps=getreqrps();
    if (req_rps != 0) {
        if (orienttype() == invalid) {
            int diff = req_rps - getcurrrps();
            if (diff > 5) motor_speed_val += 0.5;
            else if (diff > 3.5 && diff <= 5) motor_speed_val += 0.3;
            else if (diff > 1 && diff <= 3.5) motor_speed_val += 0.01;
            else if (diff < 0 && diff >= -2) motor_speed_val -= 0.01;
            else if (diff < (-2)) motor_speed_val -= 0.05;
        }
        else if (orient == uphill) {
            int diff = req_rps - getcurrrps();
            if (diff > 5) motor_speed_val = 19.3;
            else if (diff > 3.5 && diff <= 5) motor_speed_val += 0.3;
            else if (diff > 1 && diff <= 3.5) motor_speed_val += 0.01;
            else if (diff < 0 && diff >= -2) motor_speed_val -= 0.01;
            else if (diff < (-2)) motor_speed_val -= 0.05;
        }
        else if (orient == downhill) {
            int diff = getcurrrps() - req_rps;
            if (diff > 8) motor_speed_val = 18.3;
            else if (diff > 4) motor_speed_val = 18.7;
            //if (diff > 4) motor_speed_val -= 1;
            else if (diff > 3.5) motor_speed_val -= 0.3;
            else if (diff > 1) motor_speed_val -= 0.01;
            else if (diff < 0) motor_speed_val += 0.005;
        }
        motor_speed.set(motor_speed_val);
        car.setmotorspeed(motor_speed_val);
    }
    return motor_speed_val;
}

void maintainspeed()
{
    int revolutions = car.getRevolutions();
    calculate_speed(revolutions);
    feedback();
    car.setRevolutions(0);
    //get kph and send it to CAN separately
}

void calc_orientation()
{
    if(reverse_flag) reverse_init();


    tilt_x = AS.getX();
    tilt_y=AS.getY();

    if(tilt_x>46) {  //&& tilt_x>tilt_y  //old value - x>76
        //uphill
        LE.off(1);
        LE.off(2);
        orient=uphill;
        LE.toggle(4);
    }
    else if(tilt_x<(-150)) { //&& tilt_x<tilt_y) //old val x<-150
        //downhill
        LE.off(4);
        LE.off(2);
        orient=downhill;
        LE.toggle(1);
    }
    else {
        //invalid
        LE.off(1);
        LE.off(4);
        orient=invalid;
        LE.toggle(2);
    }

    if(IGNORE_ORIENT)
        orient=invalid;
    //return value for unit testing

/*    if(SW.getSwitch(1))
        START_OP=true;
    else if(SW.getSwitch(2))
        START_OP=false;*/
}

bool checkmia()
{
    if (dbc_handle_mia_MASTER_SPEED(&speed, 250)) {
        LD.setNumber(99);
        return true;
    }
    return false;
}

int mspeed(uint8_t*a)
{
    int s;
    s = *a;
    //	car.setspeed(s);
    return s;
}

int mdirection(uint8_t*b)
{
    int d;
    d = *b;
    //	car.setdirection(d);
    return d;
}

bool receive_decode_MID(uint8_t*a, uint8_t*b){
    while ((CAN_rx(can1, &motor_message, 0)) && (!reverse_flag)) {
        can_msg_hdr.dlc = motor_message.frame_fields.data_len;
        can_msg_hdr.mid = motor_message.msg_id;

        if(can_msg_hdr.mid == 80);
            //START_OP = true;
        else if(can_msg_hdr.mid == 81);
            //START_OP=false;
        else if(can_msg_hdr.mid == 203){
            dbc_decode_MASTER_SPEED(&speed, motor_message.data.bytes, &can_msg_hdr);
            *a = speed.MASTER_Maintain_Speed;
            *b = speed.MASTER_Maintain_Direction;

        }
        return true;
    }
    return false;
}


bool recieveanddecode(uint8_t*a, uint8_t*b)
{

    while ((CAN_rx(can1, &motor_message, 0)) && (!reverse_flag)) {
        can_msg_hdr.dlc = motor_message.frame_fields.data_len;
        can_msg_hdr.mid = motor_message.msg_id;

        if (can_msg_hdr.mid == 203) {
            dbc_decode_MASTER_SPEED(&speed, motor_message.data.bytes, &can_msg_hdr);
            *a = speed.MASTER_Maintain_Speed;
            *b = speed.MASTER_Maintain_Direction;
            return true;
        }
        else
            return false;
    }
    return false;
    //enum for msg ID
}

void feedback_servo(){

    int diff_servo= req_servo_dir_val-servo_dir_val;

    if(diff_servo!=0){

        if(diff_servo<-8||diff_servo>8){
            servo_dir_val=SERVO_CENTER;
        }else if(diff_servo>-8 && diff_servo<-4){
            servo_dir_val-=0.02;
        }else if(diff_servo>-4 && diff_servo<0){
            servo_dir_val-=0.01;
        }else if(diff_servo>0 && diff_servo<4){
            servo_dir_val+=0.01;
        }else if(diff_servo>4 && diff_servo<8){
            servo_dir_val+=0.02;
        }

    }

    if(servo_dir_val>=25){
        servo_dir_val=25;
    }else if(servo_dir_val<=10.9){
        servo_dir_val=10.9;
    }
    motor_dir.set(servo_dir_val);

}

int getrps(uint8_t speed_curr)
{
    if (car.getspeed() != speed_curr) {
        if (reverse_flag) motor_speed.set(18.6);
        switch (speed_curr) {
            case 0:
                //reverse
                req_rps = 0;
                reverse_flag = true;
                reverse_count = 5;
                //apply_brakes();
                break;
            case 1:
                //brakes
                motor_speed_val = BRAKES;
                req_rps = 0;
                apply_brakes();
                reverse_flag = false;
                brake_flag = true;

                break;
            case 2:
                //forward slow
                //if (reverse_flag)
                //motor_speed.set(MOTOR_INIT);
                //motor_speed_val=MOTOR_INIT;
                req_rps = 6;
                reverse_flag = false;
                break;
            case 3:
                //fast
                //if (reverse_flag)
                //motor_speed.set(MOTOR_INIT);
                //motor_speed_val=MOTOR_INIT;
                req_rps = 6;        //good for flat and uphill 12;
                reverse_flag = false;
                break;
            case 4:
                //turbo
                break;
            case 5:
                //do nothing
                break;
            default:
                break;
                //MIA
        }
        car.setspeed(speed_curr);
    }
    return req_rps;
}

float getdir(uint8_t dir_curr)
{
    if (dir_curr != car.getdirection()) {
        switch (dir_curr) {
            case 0:
                //center
                servo_dir_val = SERVO_CENTER;
                break;
            case 1:
                //hard left 10.9
                servo_dir_val = SERVO_HARD_LEFT;
                //servo_dir_val = SERVO_SLIGHT_LEFT;
                break;
            case 2:
                //slight-left 14.9
                servo_dir_val = SERVO_SLIGHT_LEFT;
                break;
            case 3:
                //hard right 25
                servo_dir_val = SERVO_HARD_RIGHT;
                //servo_dir_val = SERVO_SLIGHT_RIGHT;
                break;
            case 4:
                //slight right 20.7
                servo_dir_val = SERVO_SLIGHT_RIGHT;
                break;
        }
        car.setdirection(dir_curr);
        car.setmotordir(servo_dir_val);
    }
    return req_servo_dir_val;
}

bool getcanmsg()
{
    uint8_t a, b;
    uint8_t speed_curr = 0;
    uint8_t dir_curr = 0;

/*    if(receive_decode_MID(&a, &b)){

        if(START_OP){
        speed_curr = mspeed(&a);
        dir_curr = mdirection(&b);

        req_rps = getrps(speed_curr);
        servo_dir_val = getdir(dir_curr);
        }
    }*/

    if (recieveanddecode(&a, &b)) {
        speed_curr = mspeed(&a);
        req_rps = getrps(speed_curr);
        dir_curr = mdirection(&b);
        servo_dir_val = getdir(dir_curr);
    }
    return true;

    //if needed change receivedecode as two functions
}

void apply_brakes()
{
    /*	 if ((req_rps == 0)) {
     if(reverse_flag)
     {
     reverse_flag = false;
     motor_speed.set(MOTOR_INIT);
     }
     else
     {
     motor_speed.set(BRAKES);
     }
     motor_speed_val = 18.9;
     car.setmotorspeed(motor_speed_val);
     }*/

    if (car.getspeed() == 0) motor_speed.set(18.6);
    else
        motor_speed.set(BRAKES);

    motor_speed_val = 18.9;
    car.setmotorspeed(motor_speed_val);
}
void setmotorPWM()
{
    getcanmsg();
    checkmia();        //put directly, function not req
    motor_dir.set(car.getmotordir());
}

void PWMcontrol()
{
    if (motor_speed_val > 20) motor_speed_val = 20;
    else if (motor_speed_val < 17) motor_speed_val = 17;
}

