#ifndef DRIVER_HPP_
#define DRIVER_HPP_

#include <stdint.h>

typedef enum {
	uphill, downhill, invalid
} orientation_t;

void initialization(void);
void bus_off(uint32_t);
void data_ovr(uint32_t);
void count_rpm();
bool calculate_speed();
void setmotorPWM();
float feedback(uint32_t req_rps);
void maintainspeed();
void calc_orientation();
bool getcanmsg();
bool checkmia();
void apply_brakes();
bool recieveanddecode(uint8_t*a, uint8_t*b);
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
