class Car {
  int choice, dist, designated;
  public Car(int choice, int dist, int designated) {
    this.choice = choice;
    this.dist = dist;
    this.designated = designated;
  }
}

int readDir(String s) {
  if (s.equals("N"))
    return DIR_N;
  else if (s.equals("S"))
    return DIR_S;
  else if (s.equals("W"))
    return DIR_W;
  else if (s.equals("E"))
    return DIR_E;
  else
    return -1;
}

boolean canMove(Car car, List<Car> cars) {
 for (Car c: cars) {
   if (c == car || c.choice != car.choice)
     continue;
   
   if (c.dist - 1 == car.dist)
     return false;
 }
 return true;
}