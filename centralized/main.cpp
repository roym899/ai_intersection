#include <iostream>
#include <string>
#include <utility>
#include <queue>
#include "intersection.h"
#include "car.h"


int main() {
    /* read */
    std::string input;
    // read transition matrix
    
    double initial_velocity = 0;
    double max_velocity = 0;
    double max_acceleration = 0;
    double lane_length = 0;
    double time_step = 0;
    double timestamp = 0;
    std::string lane = "";
    std::string goal = "";

    // queue that saves all the cars and their respective time_stamps
    std::queue<std::pair<Car,double>> car_queue;

    int counter = 0;
    bool property = false;
    int car_counter = 0;
    while(std::cin >> input) {
        property = false;
        switch (counter){
            case 0: initial_velocity = std::stod(input); property = true ; break;
            case 1: max_velocity =  std::stod(input); property = true ; break;
            case 2: max_acceleration =  std::stod(input); property = true ; break;
            case 3: lane_length =  std::stod(input); property = true ; break;
            case 4: time_step =  std::stod(input); property = true ; break;
            default: property = false; break;
        }
        counter++;
        if(property){
            continue;
        }   

        if(isdigit(input.at(0))){
            timestamp = std::stod(input);
            car_counter = 0;    
        }
        else{
            if (car_counter == 0){
                lane = input;
                car_counter = 1;
            }
            else if(car_counter == 1){
                goal = input;
                car_counter = 0;
                car_queue.push(std::make_pair(Car(initial_velocity, lane[0], goal[0]),timestamp));
            }
            else{
                std::cout << "Error" << std::endl;
                break;
            }
            
        }
    }


    // Create new Intersection
    Intersection intersection(max_velocity,max_acceleration,lane_length);
    timestamp = 0;
    bool current_timestamp = true;
    while(!intersection.is_empty() || car_queue.size() != 0) {
        current_timestamp = true;
        while(current_timestamp){
            if(car_queue.empty()){
                break;
            }
            std::pair<Car,double> current_car_pair = car_queue.front();
            if(current_car_pair.second == timestamp){
                intersection.add_car(current_car_pair.first);
                car_queue.pop();
            }
            else{
                current_timestamp = false;
            }
            
        }
        

        timestamp = intersection.sim_step(time_step);

        std::cout << intersection << std::endl;
    }
    return 0;
}