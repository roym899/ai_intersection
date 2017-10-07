#include <iostream>
#include "intersection.h"
int main() {
	/* read */
	std::string input;
	// read transition matrix
	int counter = 0;
	bool property = false;
	double initial_velocity = 0;
	double max_velocity = 0;
	double max_acceleration = 0;
	double lane_length = 0;
	double time_step = 0;
  	while(std::cin >> input) {
  		property = false;
  		switch (counter){
  			case 0:	initial_velocity = stod(input); property = true ; break;
  			case 1:	max_velocity = stod(input); property = true ; break;
  			case 2:	max_acceleration = stod(input); property = true ; break;
  			case 3:	lane_length = stod(input); property = true ; break;
  			case 4: time_step = stod(input); property = true ; break;
  			default: property = false; break;
  		}
  		counter++;
  		if(property){
  			continue;
  		}		
  		
  	}


	// Create new Intersection
	Intersection intersection(max_velocity,max_acceleration,lane_length);
	double timestamp = 0;

	while(!intersection.is_clear() || !car_queue.clear()) {
		intersection.add_car(Car(initial_velocity));

		timestamp = intersection.sim_step(time_step);
	}
	
}