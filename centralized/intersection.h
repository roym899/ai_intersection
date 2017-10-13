#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <vector>
#include <iostream>

#include "car.h"
#include "csp_solver.h"


class Intersection
{
public:
	Intersection(double max_velocity, double max_acceleration, double lane_length, bool fcfs);
	~Intersection();

	// checks if there is still a car on the intersection
	bool is_empty();
	

	// simulates time_step starting from the current state
	// returns the current timestamp
	double sim_step(double time_step);

	// sets the right acceleration for every car such that it arrives at the intersection at the current goal time
	void cruise_control(double time_step);

	// adds a car to the intersection
	void add_car(const Car &car);

	// returns a sequence of integers representing the sequence of squares on the intersection
	std::vector<int> get_bin_sequence_for_car(const Car &car);

	// output operator to bring the current intersection state to the right format for the visualizer
	friend std::ostream& operator<< (std::ostream& stream, const Intersection& intersection);

private:
	void update_fields();

	std::vector<Car> cars;
	std::vector<double> remaining_times;
	int fields[4];
	bool replan;

	double timestamp;
	double max_velocity;
	double max_acceleration;
	double lane_length;
	double fcfs_remaining_time;
	bool fcfs;
};

#endif
