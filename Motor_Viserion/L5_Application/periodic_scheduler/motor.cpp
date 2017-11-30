#include "motor.hpp"
#include <stdint.h>

#define MOTOR_INIT          18
#define SERVO_INIT			18

motor::motor() {
	motorspeed = MOTOR_INIT;
	motordir = SERVO_INIT;
	Revolutions = 0;
	kph = 0;
	speed=0;
	direction=0;
}

void motor::incrRevolutions(void) {
	++Revolutions;
	return;
}

int motor::getRevolutions() {
	return Revolutions;
}

float motor::getkph() {
	return kph;
}

uint8_t motor::getspeed() {
	return speed;
}

uint8_t motor::getdirection() {
	return direction;
}

void motor::setdirection(uint8_t d) {
	direction = d;
	return;
}

void motor::setspeed(uint8_t c) {
	speed = c;
	return;
}

void motor::setRevolutions(int a) {
	Revolutions = a;
	return;
}

void motor::setkph(float b) {
	kph = b;
	return;
}

void motor::setmotorspeed(float e) {
	motorspeed = e;
	return;
}

void motor::setmotordir(float f) {
	motordir = f;
	return;
}

float motor::getmotorspeed() {
	return motorspeed;
}

float motor::getmotordir() {
	return motordir;
}

void motor::setmspeedincr(float g) {
	motorspeed = motorspeed + g;
	return;
}

void motor::setmspeeddec(float h) {
	motorspeed = motorspeed + h;
	return;
}
