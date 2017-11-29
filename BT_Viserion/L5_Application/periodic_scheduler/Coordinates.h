#ifndef L5_APPLICATION_PERIODIC_SCHEDULER_COORDINATES_H_
#define L5_APPLICATION_PERIODIC_SCHEDULER_COORDINATES_H_

class Coordinates {
public:
	Coordinates();
	virtual ~Coordinates();

	//getters
	double get_lattitude()const;
	double get_longitude()const;

	//setters
	void set_lattitude(double value);
	void set_longitude(double value);



private:
	float lattitude;
	float longitude;
};

#endif /* L5_APPLICATION_PERIODIC_SCHEDULER_COORDINATES_H_ */
