#include "intersection.h"

#include <cmath>
#include <array>
#include <limits>

// constant time a car spends at an intersection center square
const double field_time = 2.0;


Intersection::Intersection(double max_velocity, double max_acceleration, double lane_length, bool fcfs) 
    : timestamp(0), 
      lane_length(lane_length), 
      max_velocity(max_velocity),
      max_acceleration(max_acceleration),
      replan(false),
      fcfs_remaining_time(0),
      fcfs(fcfs) {
    for(int i=0; i<sizeof(fields)/sizeof(int); ++i) 
        fields[i] = 0;
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
    // update remaining times
    for(double& remaining_time : remaining_times) {
        remaining_time -= time_step;
    }
    fcfs_remaining_time -= time_step;

    // replan for current situation
    if(replan && fcfs) {
        for(Car &car : cars) {
            // car has just been added
            if(car.current_distance == lane_length) { 
                std::vector<int> bin_sequence = get_bin_sequence_for_car(car);
                std::vector<double> length_sequence(bin_sequence.size(), field_time);
                double length_sum = 0;
                for (double length: length_sequence)
                    length_sum += length;

                // calculate minimum arrival time

                // assume max acceleration until max velocity is reached
                double t_max_vel = ( max_velocity - car.current_velocity ) / max_acceleration;
                double max_vel_dist = car.current_distance - car.current_velocity * t_max_vel - max_acceleration / 2.0 * pow(t_max_vel, 2);

                // can not reach max vel before intersection, thus assume max acceleration until hitting the intersection
                double t_min;
                if (max_vel_dist < 0) {
                    // solution can be derived by pq-formula
                    t_min = -car.current_velocity/max_acceleration + sqrt(pow(car.current_velocity, 2)/pow(max_acceleration, 2) + 2.0*car.current_distance/max_acceleration);
                }
                else {
                    t_min = 1.0 / max_velocity * max_vel_dist + t_max_vel;
                }

                if(t_min > fcfs_remaining_time) {
                    remaining_times.push_back(t_min);
                    fcfs_remaining_time = t_min + length_sum;
                }
                else {
                    remaining_times.push_back(fcfs_remaining_time);
                    fcfs_remaining_time += length_sum;
                }
            }
        }
    }
    else if(replan && !fcfs) {// a car has been added => replan for new situation
        CSP_Solver solver;
        std::array<double, 4> previous_min_time;

        for(int i=0; i<4; ++i)
            previous_min_time[i] = 0;


        for(Car &car : cars) {
            // add cars which have not hit the intersection yet as solving variables to the intersection
            if(car.current_distance > 0) {
                std::vector<int> bin_sequence = get_bin_sequence_for_car(car);
                std::vector<double> length_sequence(bin_sequence.size(), field_time);

                // assume max acceleration until max velocity is reached
                double t_max_vel = ( max_velocity - car.current_velocity ) / max_acceleration;
                double max_vel_dist = car.current_distance - car.current_velocity * t_max_vel - max_acceleration / 2.0 * pow(t_max_vel, 2);

                // can not reach max vel before intersection, thus assume max acceleration until hitting the intersection
                double t_min;
                if (max_vel_dist < 0) {
                    // solution can be derived by pq-formula
                    t_min = -car.current_velocity/max_acceleration + sqrt(pow(car.current_velocity, 2)/pow(max_acceleration, 2) + 2.0*car.current_distance/max_acceleration);
                }
                else {
                    t_min = 1.0 / max_velocity * max_vel_dist + t_max_vel;
                }

                // calculate max time to hit the intersection
                double max_deceleration = - max_acceleration;
                double t_vel_0 = -car.current_velocity / max_deceleration;
                double distance_vel_0 = car.current_distance - max_deceleration / 2 * pow(t_vel_0, 2) - car.current_velocity * t_vel_0;

                double t_max;
                // it is possible to stop before the intersection => infinite start time possible
                if(distance_vel_0 > 0)
                    t_max = std::numeric_limits<double>::max();
                else 
                    t_max = - car.current_velocity / max_deceleration - sqrt(pow(car.current_velocity, 2)/pow(max_deceleration, 2)+2.0*car.current_distance/max_deceleration);

                // time to hit intersection the earliest to keep everything in order 
                if(t_min < previous_min_time[*bin_sequence.begin()])
                    t_min = previous_min_time[*bin_sequence.begin()]+2;

                std::cerr << car.current_velocity << " " << car.current_distance << " " << t_max << " " << t_min << std::endl;
                solver.add_range_bin_sequence(t_min ,t_max, bin_sequence, length_sequence);
                previous_min_time[*bin_sequence.begin()] = t_min;
            }
            else { // car on intersection already
                std::vector<int> bin_sequence = get_bin_sequence_for_car(car);

                double passed = car.current_field_time / field_time;
                double current_time = car.current_field_time;

                if(passed > 1) {
                    current_time -= field_time;
                    bin_sequence.erase(bin_sequence.begin());
                }
                if(passed > 2) {
                    current_time -= field_time;
                    bin_sequence.erase(bin_sequence.begin());
                }

                std::vector<Range> range_sequence;
                double start_time = 0;
                double end_time = field_time-current_time;

                for(int bin : bin_sequence) {
                    range_sequence.push_back(Range(start_time, end_time));
                    start_time = end_time;
                    end_time += field_time;
                }

                for(int i=0; i<bin_sequence.size(); ++i) {
                    solver.add_range_bin_constraint(bin_sequence[i], range_sequence[i]);            
                }
                
            }
        }

        std::cerr << "Solve..." << std::endl;
        std::vector<double> times = solver.solve();

        replan = false;

        // SIMPLE VERSION TO TEST SCHEDULING: 
        //   just set the velocity to the average velocity to hit the intersection
        /*int count = 0;
        for(Car &car : cars) {
            if(car.current_distance > 0) {
                car.current_velocity = car.current_distance / times[count++];
            }
        }*/

        // solver returns times vector without the cars

        remaining_times = times;
    }

    // adjust the cars velocity
    cruise_control(time_step);

    // simulate all cars
    auto remaining_times_it = remaining_times.begin();
    for(Car &car : cars) {
        // car approaching intersection
        if(car.current_distance > 0) {
            car.current_velocity += car.current_acceleration*time_step;
            car.current_distance -= car.current_velocity*time_step;
            // car entering discretized intersection
            if(car.current_distance <= 0) {
                car.current_field_time = (-car.current_distance/car.current_velocity);

                // remove from remaining_times list to only keep entries for cars still driving
                remaining_times_it = remaining_times.erase(remaining_times_it);
            }
            else 
                ++remaining_times_it;
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

    // init all fields with no car on it
    for(int i=0; i<sizeof(fields)/sizeof(int); ++i) 
        fields[i] = 0;

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
                if(fields[idx] != 0)
                    std::cerr << "Error @" << timestamp << ": field " << idx << " is already occupied!!" << std::endl;
                else
                    fields[idx] = it->id;
                ++it;
            }
        }
        else
            ++it;
    }   
}


// Sets the needed acceleration to reach the intersection at the designated time
// needs to be called every simulation step.

void Intersection::cruise_control(double time_step){
    int car_counter = 0;
    int time_difference = 0;
    double v_need = 0;
    double a_need = 0;
    double next_velocity = 0;

    for(Car &car : cars){
        if(car.current_distance > 0) {
            v_need = car.current_distance / remaining_times[car_counter];
            // adjust the acceleration according to the current situation
            if (v_need == car.current_velocity){
                car.current_acceleration = 0;
            }
            else if(v_need > car.current_velocity)
                car.current_acceleration = (v_need - car.current_velocity) / time_step > max_acceleration ? max_acceleration : (v_need - car.current_velocity) / time_step;
            else if(v_need < car.current_velocity)
                car.current_acceleration = (v_need - car.current_velocity) / time_step > -max_acceleration ? (v_need - car.current_velocity) / time_step : -max_acceleration; 
            
            car_counter++;

            if (v_need > max_velocity*1.01) {
                std::cerr << "Constraint violated " << v_need << " " << max_velocity  << std::endl;
            }
        }
    }
}

std::ostream& operator<< (std::ostream& stream, const Intersection& intersection) {
    // intersection variables
    stream << intersection.lane_length << " " << intersection.timestamp << " ";


    // current intersection state
    for(int i=0; i<sizeof(intersection.fields)/sizeof(int); ++i)  {
        stream << intersection.fields[i] << " ";
    }

    char lanes[] = {'N', 'E', 'S', 'W'};

    for(int i=0; i<sizeof(lanes); ++i) {
        stream << lanes[i] << " ";
        for(const Car &car : intersection.cars) {
            if(car.lane == lanes[i])
                stream << car.id << " " << car.current_distance << " ";
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
