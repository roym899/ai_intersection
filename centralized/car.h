#ifndef CAR_H
#define CAR_H
class Car
{
public:
	Car(double initial_velocity, char lane, char goal);
	Car(const Car &car);
	~Car();
	
	char lane;
	char goal;
	double current_distance;
	double current_field_time;
	double current_velocity;
	double current_acceleration;

};



#endif