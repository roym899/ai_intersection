import java.util.Comparator;

import java.util.Collections;
import java.util.List;
import java.util.Map;

final int CELL_NW = 0, CELL_NE = 1, CELL_SW = 2, CELL_SE = 3;
final int DIR_N = 0, DIR_S = 1, DIR_W = 2, DIR_E = 3;

void settings() {
  size(350, 350);
}

/* Fields */

Map<Integer, List<Car>> cars;
int laneLength;

void setup() {
  frameRate(2);
  noStroke();
  
  String[] lines = loadStrings("simulation.txt");
  
  cars = new HashMap<Integer, List<Car>>();
  cars.put(0, new ArrayList<Car>());
  cars.put(1, new ArrayList<Car>());
  cars.put(2, new ArrayList<Car>());
  cars.put(3, new ArrayList<Car>());

  laneLength = Integer.parseInt(lines[0]);
  
  for (int i = 1; i < lines.length; ++i) {
    String[] splitted = lines[i].split("\\s+");
    if (splitted.length != 4)
      throw new Error();
    
    int lane = readDir(splitted[0]);
    int choice = readDir(splitted[1]);
    
    int dist = Integer.parseInt(splitted[2]);
    int designated = Integer.parseInt(splitted[3]);
    
    cars.get(lane).add(new Car(choice, dist, designated));
  }
  
  Comparator<Car> asc = new Comparator<Car>() {
    public int compare(Car i1, Car i2) {
      return Integer.compare(i1.dist, i2.dist);
    }
  };
    
  Collections.sort(cars.get(0), asc);
  Collections.sort(cars.get(1), asc);
  Collections.sort(cars.get(2), asc);
  Collections.sort(cars.get(3), asc);
}

void draw() {
  background(127, 127, 127);
  
  fill(255, 255, 255);
  rect(150, 0, 50, 350);
  rect(0, 150, 350, 50);
  
  translate(width / 2, height / 2);
  
  // Show cars
  for (Map.Entry<Integer, List<Car>> entry: cars.entrySet()) {
    pushMatrix();
    switch (entry.getKey()) {
      case DIR_N:
        rotate(PI);
        break;
      case DIR_S:
        break;
      case DIR_W:
        rotate(PI / 2);
        break;
      case DIR_E:
        rotate(-PI / 2);
        break;
    }
    
    for (Car car: entry.getValue()) {
      float ratio = car.dist * 1f / laneLength;
      fill(153, 51, 51);
      rect(0, 25 + ratio * 150, 25, 150 * 1f / laneLength);
    }
    
    popMatrix();
  }
}