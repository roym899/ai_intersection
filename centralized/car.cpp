#include "car.h"

#include <iostream>

int Car::car_counter = 1;

Car::Car(double initial_velocity, char lane, char goal) 
	: lane(lane),
	  goal(goal),
	  current_distance(0), 
	  current_velocity(initial_velocity), 
	  current_acceleration(0),
	  current_field_time(0),
	  id(car_counter++) { 
	  	std::cerr << "car: " << car_counter << std::endl;
	
}

Car::Car(const Car &car) 
	: lane(car.lane),
	  goal(car.goal),
	  current_distance(car.current_distance), 
	  current_velocity(car.current_velocity), 
	  current_acceleration(car.current_acceleration),
	  current_field_time(car.current_field_time),
	  id(car.id) {

}

Car::~Car() {
	
}