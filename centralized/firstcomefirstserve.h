
#ifndef FIRSTCOMEFIRSTSERVE_H
#define FIRSTCOMEFIRSTSERVE_H


#include <vector>
#include <iostream>
#include "car.h"

class Firstcomefirstserve
{
public:
	Firstcomefirstserve();
	~Firstcomefirstserve();

	void add_min_starts_length(double min_start,std::vector<double> length_sequence);
	std::vector<double> solve();
private:

	std::vector<double> min_starts;
	std::vector<double> lengths;
	
};

#endif