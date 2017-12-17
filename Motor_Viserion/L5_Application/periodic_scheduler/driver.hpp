#ifndef DRIVER_HPP_
#define DRIVER_HPP_

#include <stdint.h>

typedef enum {
	uphill, downhill, invalid
} orientation_t;

void initializeCAN();
void initializeCAR(void);
void bus_off(uint32_t);
void data_ovr(uint32_t);
void count_rpm();
bool calculate_speed(int no_of_rev);
void setmotorPWM();
float feedback();
void feedback_servo();
void maintainspeed();
void calc_orientation();
bool getcanmsg();
bool checkmia();
void apply_brakes();
void PWMcontrol();
bool recieveanddecode(uint8_t*a, uint8_t*b);
bool receive_decode_MID(uint8_t*a, uint8_t*b);
int mspeed(uint8_t*a);
int mdirection(uint8_t*b);
int getrps(uint8_t speed_curr);
float getdir(uint8_t dir_curr);
bool ifreverse();
int orienttype();
uint32_t getcurrrps();
int getreqrps();
void reverse_init();

#endif /* DRIVER_HPP_ */
