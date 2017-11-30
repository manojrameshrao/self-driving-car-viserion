#ifndef MOTOR_HPP__
#define MOTOR_HPP__

#include <stdint.h>

class motor {
public:
	motor();
	int getRevolutions();
	float getkph();
	uint8_t getspeed();
	uint8_t getdirection();
	float getmotorspeed();
	float getmotordir();
	void incrRevolutions(void);
	void setRevolutions(int a);
	void setkph(float b);
	void setspeed(uint8_t c);
	void setdirection(uint8_t d);
	void setmotorspeed(float e);
	void setmotordir(float f);
	void setmspeedincr(float g);
	void setmspeeddec(float h);

private:
	int Revolutions;
	uint8_t speed;
	uint8_t direction;
	float kph;
	float motorspeed;
	float motordir;
};

#endif /* DCMOTOR_HPP__ */
