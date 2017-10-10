#include "intersection.h"

#include <cmath>
#include <array>

const double field_time = 2.0;

Intersection::Intersection(double max_velocity, double max_acceleration, double lane_length) 
	: timestamp(0), 
	  lane_length(lane_length), 
	  max_velocity(max_velocity),
	  max_acceleration(max_acceleration),
	  replan(false) {
	for(int i=0; i<sizeof(fields); ++i) 
		fields[i] = false;
}

Intersection::~Intersection() {

}

bool Intersection::is_empty() {
	return (cars.size() == 0) ? true : false;
}


void Intersection::add_car(const Car &car) {
	cars.push_back(car);

	(cars.end()-1)->current_distance = lane_length;

	replan = true;
}

// simulates time_step starting from the current state
// returns the current timestamp
double Intersection::sim_step(double time_step) {
	// replan for current situation
	if(replan) {// a car has been added => replan for new situation
		CSP_Solver solver;
		std::array<double, 4> previous_min_time;

		for(int i=0; i<4; ++i)
			previous_min_time[i] = 0;


		for(Car &car : cars) {
			std::vector<int> bin_sequence = get_bin_sequence_for_car(car);
			std::vector<double> length_sequence(bin_sequence.size(), field_time);

			double t_max_vel = ( max_velocity - car.current_velocity ) / max_acceleration;
			double max_vel_dist = car.current_distance - car.current_velocity * t_max_vel - max_acceleration / 2 * pow(t_max_vel, 2);
			double t_min = 1 / max_velocity * max_vel_dist + t_max_vel;

			if(t_min > previous_min_time[*bin_sequence.begin()]+2) {
				solver.add_range_bin_sequence(t_min ,100000, bin_sequence, length_sequence);
				previous_min_time[*bin_sequence.begin()] = t_min; // time to hit intersection the earliest to keep everything in order
			}
			else {
				solver.add_range_bin_sequence(previous_min_time[*bin_sequence.begin()]+2 ,100000, bin_sequence, length_sequence);
				previous_min_time[*bin_sequence.begin()] = previous_min_time[*bin_sequence.begin()]+2; // time to hit intersection the earliest to keep everything in order	
			}
		}

		std::cerr << "Solve..." << std::endl;
		std::vector<double> times = solver.solve();

		replan = false;

		int count = 0;
		for(Car &car : cars)
			car.current_velocity = car.current_distance / times[count++];
	}

	// simulate all cars
	for(Car &car : cars) {
		// car approaching intersection
		if(car.current_distance > 0) {
			car.current_distance -= car.current_velocity*time_step;
			car.current_velocity += car.current_acceleration*time_step;
			// car entering discretized intersection
			if(car.current_distance <= 0) {
				car.current_field_time = (-car.current_distance/car.current_velocity);
			}
		}
		else {
			car.current_field_time += time_step;
		}
	}

	update_fields();

	// update the timestamp
	timestamp += time_step;
	return timestamp;
}

void Intersection::update_fields() {

	// init all fields with false
	for(int i=0; i<sizeof(fields); ++i) 
		fields[i] = false;

	for(std::vector<Car>::iterator it=cars.begin(); it != cars.end();) {
		if(it->current_distance <= 0) { // it->is on intersection
			bool remove=false;
			int idx=-1;
			if(it->current_field_time / field_time <= 1.0) {
				if(it->lane == 'N')
					idx = 0;
				else if(it->lane == 'E')
					idx = 1;
				else if(it->lane == 'S')
					idx = 3;
				else if(it->lane == 'W')
					idx = 2;
			}
			else if(it->current_field_time / field_time <= 2.0) {
				if(it->lane == 'N' && it->goal == 'W')
					remove = true;
				else if(it->lane == 'N')
					idx = 2;
				else if(it->lane == 'E' && it->goal == 'N')
					remove = true;
				else if(it->lane == 'E')
					idx = 0;
				else if(it->lane == 'S' && it->goal == 'E')
					remove = true;
				else if(it->lane == 'S')
					idx = 1;
				else if(it->lane == 'W' && it->goal == 'S')
					remove = true;
				else if(it->lane == 'W')
					idx = 3;
			}
			else if(it->current_field_time / field_time <= 3.0) {
				if(it->lane == 'N' && it->goal == 'S')
					remove = true;
				else if(it->lane == 'N')
					idx = 3;
				else if(it->lane == 'E' && it->goal == 'W')
					remove = true;
				else if(it->lane == 'E')
					idx = 2;
				else if(it->lane == 'S' && it->goal == 'N')
					remove = true;
				else if(it->lane == 'S')
					idx = 0;
				else if(it->lane == 'W' && it->goal == 'E')
					remove = true;
				else if(it->lane == 'W')
					idx = 1;
			}
			else {
				remove = true;
			}

			if (remove) {
				it = cars.erase(it);

			}
			else {
				if(idx == -1)
					std::cerr << "Error: idx has not been set" << std::endl;
				if(fields[idx])
					std::cerr << "Error: field is already occupied!!" << std::endl;
				else
					fields[idx] = true;
				++it;
			}
		}
		else
			++it;
	}	
}

std::ostream& operator<< (std::ostream& stream, const Intersection& intersection) {
	// intersection variables
	stream << intersection.lane_length << " " << intersection.timestamp << " ";


	// current intersection state
	for(int i=0; i<sizeof(intersection.fields); ++i)  {
		if (intersection.fields[i])
			stream << "1 ";
		else 
			stream << "0 ";
	}

	char lanes[] = {'N', 'E', 'S', 'W'};

	for(int i=0; i<sizeof(lanes); ++i) {
		stream << lanes[i] << " ";
		for(const Car &car : intersection.cars) {
			if(car.lane == lanes[i])
				stream << car.current_distance << " ";
		}	
	}

	return stream;
}


std::vector<int> Intersection::get_bin_sequence_for_car(const Car &car) {
	std::vector<int> bin_sequence;

	if(car.lane == 'N' && car.goal == 'E') {
		bin_sequence.push_back(0);
		bin_sequence.push_back(2);
		bin_sequence.push_back(3);
	}
	else if(car.lane == 'N' && car.goal == 'S') {
		bin_sequence.push_back(0);
		bin_sequence.push_back(2);
	}
	else if(car.lane == 'N' && car.goal == 'W') {
		bin_sequence.push_back(0);
	}
	else if(car.lane == 'W' && car.goal == 'N') {
		bin_sequence.push_back(2);
		bin_sequence.push_back(3);
		bin_sequence.push_back(1);
	}
	else if(car.lane == 'W' && car.goal == 'E') {
		bin_sequence.push_back(2);
		bin_sequence.push_back(3);
	}
	else if(car.lane == 'W' && car.goal == 'S') {
		bin_sequence.push_back(2);
	}
	else if(car.lane == 'S' && car.goal == 'W') {
		bin_sequence.push_back(3);
		bin_sequence.push_back(1);
		bin_sequence.push_back(0);
	}
	else if(car.lane == 'S' && car.goal == 'N') {
		bin_sequence.push_back(3);
		bin_sequence.push_back(1);
	}
	else if(car.lane == 'S' && car.goal == 'E') {
		bin_sequence.push_back(3);
	}
	else if(car.lane == 'E' && car.goal == 'E') {
		bin_sequence.push_back(1);
		bin_sequence.push_back(0);
		bin_sequence.push_back(2);
	}
	else if(car.lane == 'E' && car.goal == 'W') {
		bin_sequence.push_back(1);
		bin_sequence.push_back(0);
	}
	else if(car.lane == 'E' && car.goal == 'N') {
		bin_sequence.push_back(1);
	}
	return bin_sequence;
}