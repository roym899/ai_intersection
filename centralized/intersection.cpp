Intersection(double max_velocity, double max_acceleration, double lane_length) 
	: timestamp(0), 
	  lane_length(lane_length), 
	  max_velocity(max_velocity),
	  max_acceleration(max_acceleration) {

}

~Intersection() {

}

bool is_empty() {

}

// simulates time_step starting from the current state
// returns the current timestamp
double sim_step(double time_step) {
	// replan for current situation

	// simulate all cars


	// update the timestamp
	timestamp += time_step;
}