class Car {
  int choice, dist, designated;
  public Car(int choice, int dist, int designated) {
    this.choice = choice;
    this.dist = dist;
    this.designated = designated;
  }
}

class Turning extends Car {
  List<Integer> cells;
  int step;
  public Turning(Car car, int lane) {
    super(car.choice, car.dist, car.designated);

    cells = new ArrayList<Integer>();
    step = 0;

    switch (lane) {
    case DIR_N:
      switch (choice) {
        case (DIR_W):
        cells.add(CELL_NW);
        break;
        case (DIR_S):
        cells.add(CELL_NW);
        cells.add(CELL_SW);
        break;
        case (DIR_E):
        cells.add(CELL_NW);
        cells.add(CELL_SW);
        cells.add(CELL_SE);
        break;
      }
      break;
    case DIR_S:
      switch (choice) {
        case (DIR_E):
        cells.add(CELL_SE);
        break;
        case (DIR_N):
        cells.add(CELL_SE);
        cells.add(CELL_NE);
        break;
        case (DIR_W):
        cells.add(CELL_SE);
        cells.add(CELL_NE);
        cells.add(CELL_NW);
        break;
      }
      break;
    case DIR_W:
      switch (choice) {
        case (DIR_S):
        cells.add(CELL_SW);
        break;
        case (DIR_E):
        cells.add(CELL_SW);
        cells.add(CELL_SE);
        break;
        case (DIR_N):
        cells.add(CELL_SW);
        cells.add(CELL_SE);
        cells.add(CELL_NE);
        break;
      }
      break;
    case DIR_E:
      switch (choice) {
        case (DIR_N):
        cells.add(CELL_NE);
        break;
        case (DIR_W):
        cells.add(CELL_NE);
        cells.add(CELL_NW);
        break;
        case (DIR_S):
        cells.add(CELL_NE);
        cells.add(CELL_NW);
        cells.add(CELL_SW);
        break;
      }
      break;
    }
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

boolean canMove(Car car, List<Car> cars, int time) {
  if (car.dist == 0) {
    if (time == car.designated)
      return true;
    else if (time < car.designated)
      return false;
    else
      throw new Error("Car did not get to intersection in time");
  }
  for (Car c : cars) {
    if (c == car)
      continue;

    if (c.dist - 1 == car.dist)
      return false;
  }
  return true;
}