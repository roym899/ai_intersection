#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <vector>
#include <iostream>

#include "car.h"


class Intersection
{
public:
	Intersection(double max_velocity, double max_acceleration, double lane_length);
	~Intersection();

	bool is_empty();
	

	// simulates time_step starting from the current state
	// returns the current timestamp
	double sim_step(double time_step);

	void add_car(const Car &car);

	friend std::ostream& operator<< (std::ostream& stream, const Intersection& intersection);

private:
	void update_fields();

	std::vector<Car> cars;
	bool fields[4];

	double timestamp;
	double max_velocity;
	double max_acceleration;
	double lane_length;
};

#endif