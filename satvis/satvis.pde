import java.util.Comparator;

import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.Iterator;

final int CELL_NW = 0, CELL_NE = 1, CELL_SW = 2, CELL_SE = 3;
final int DIR_N = 0, DIR_S = 1, DIR_W = 2, DIR_E = 3;
final int[] COLORS = {#33cccc, #3366ff, #ffff00, #ff5050};

void settings() {
  size(350, 350);
}

/* Fields */

Map<Integer, List<Car>> cars;
Map<Integer, Turning> turning;
int laneLength;
int time;

void setup() {
  frameRate(1);
  noStroke();

  String[] lines = loadStrings("simulation.txt");

  cars = new HashMap<Integer, List<Car>>();
  cars.put(0, new ArrayList<Car>());
  cars.put(1, new ArrayList<Car>());
  cars.put(2, new ArrayList<Car>());
  cars.put(3, new ArrayList<Car>());

  turning = new HashMap<Integer, Turning>();

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
  
  for (Car c : cars.get(3)) {
    println(c.dist);
  }

  time = 0;
}

void draw() {

  // Initial bs

  background(127, 127, 127);

  fill(255, 255, 255);
  rect(150, 0, 50, 350);
  rect(0, 150, 350, 50);

  // Text

  fill(0);
  textSize(12);
  text(String.format("TIME: %d", time), 0, 12);
  text("NORTH: TURQUOISE", 0, 24);
  text("SOUTH: BLUE", 0, 36);
  text("WEST: RED", 0, 48);
  text("EAST: YELLOW", 0, 60);


  translate(width / 2, height / 2);

  // Show cars

  // In intersection

  for (Map.Entry<Integer, Turning> entry : turning.entrySet()) {
    pushMatrix();
    switch (entry.getKey()) {
      case (CELL_SW):
      rotate(-PI / 2);
      case (CELL_NW):
      rotate(-PI / 2);
      case (CELL_NE):
      rotate(-PI / 2);
      case (CELL_SE):
    }
    fill(COLORS[entry.getValue().choice]);
    rect(0, 0, 25, 25);
    popMatrix();
  }

  // In lane

  for (Map.Entry<Integer, List<Car>> entry : cars.entrySet()) {
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

    for (Car car : entry.getValue()) {
      float ratio = car.dist * 1f / laneLength;
      fill(COLORS[car.choice]);
      rect(0, 25 + ratio * 150, 25, 150 * 1f / laneLength);
    }

    popMatrix();
  }

  // Prepare for next time step

  time++;

  // First make cars that are no longer needed leave the intersection

  Map<Integer, Turning> newTurning = new HashMap<Integer, Turning>();
  for (Map.Entry<Integer, Turning> entry : turning.entrySet()) {
    Turning t = entry.getValue();
    t.step++;
    if (t.step < t.cells.size())
      newTurning.put(t.cells.get(t.step), t);
  }
  turning = newTurning;

  // Then make the cars that

  for (Map.Entry<Integer, List<Car>> entry : cars.entrySet()) {
    
    List<Car> lane = entry.getValue();
    Iterator<Car> it = lane.iterator();

    while (it.hasNext()) {
      
      Car car = it.next();
      
      if (canMove(car, lane, time)) {
        
        // This means he has to leave the intersection
        if (car.dist == 0) {
          
          // Remove from lane
          it.remove();
          
          // Create intersection car
          Turning t = new Turning(car, entry.getKey());
          int cell = t.cells.get(t.step);
          
          if (!turning.containsKey(cell)) {
            turning.put(t.cells.get(t.step), t);
          } else {
            throw new Error("Cell is occupied!");
          }
          
        } else {
          car.dist--;
        }
      }
    }
  }
}