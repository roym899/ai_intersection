#include "intersection.h"



Intersection::Intersection(double max_velocity, double max_acceleration, double lane_length) 
	: timestamp(0), 
	  lane_length(lane_length), 
	  max_velocity(max_velocity),
	  max_acceleration(max_acceleration) {
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
}

// simulates time_step starting from the current state
// returns the current timestamp
double Intersection::sim_step(double time_step) {
	// replan for current situation

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
	const double field_time = 2.0;

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


// Sets the needed acceleration to reach the intersection at the designated time
// needs to be called every simulation step.
// outputs error if a constraint can't be met. --> what happens if there is a error?
void Intersection::cruise_control(const std::vector<double> arrival_times, double time_step){
	int car_counter = 0;
	int time_difference = 0;
	double v_need = 0;
	double a_need = 0;
	for(Car &car : cars){
		time_difference = arrival_times[car_counter] - time_step;
		v_need = car.current_distance / (time_difference);
		a_need = (v_need - car.current_velocity) / (time_difference);
		car.current_acceleration = a_need ; 
		car_counter++;

		if (v_need > max_velocity || a_need > max_acceleration || a_need < (-max_acceleration)){
			std::cerr << "Constraint violated" << std::endl;
		}
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