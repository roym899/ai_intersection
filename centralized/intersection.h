#include <vector>

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

	void add_car(const &Car car);

private:
	std::vector<Car> cars;
	double timestamp;
	double max_velocity;
	double max_acceleration;
	double lane_length;
};