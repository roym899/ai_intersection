#include "firstcomefirstserve.h"

void Firstcomefirstserve::add_min_starts_length(double min_start,std::vector<double> length_sequence){
	min_starts.push_back(min_start);
	double length_sum = 0;
	for (double length: length_sequence)
	{
		length_sum += length;
	}
	lengths.push_back(length_sum);
}

std::vector<double> Firstcomefirstserve::solve(){
	std::vector<double> arrival_times;
	int counter = 0;
	for (std::vector<double> &min_start: min_starts)
	{
		if(counter == 0){
			arrival_times[counter] = min_start;
		}
		else{
			if(min_start < (arrival_times[counter-1] + lengths[counter-1])){
				arrival_times[counter] = arrival_times[counter-1] + lengths[counter-1];
			}
			else{
				arrival_times[counter] = min_start;
			}
		}
		counter++;
	}
	return arrival_times;
}