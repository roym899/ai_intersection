import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.*;

/**
 * Created by hawk on 09.10.17.
 */
public class Spawner {


    public static void main(String[] args) throws IOException {

        BufferedReader stdin = new BufferedReader(new InputStreamReader(System.in));
        Random rng = new Random();

        /*
         * Read all initial values.
         *
         */

        double initVelocity = Double.parseDouble(stdin.readLine());
        double maxVelocity = Double.parseDouble(stdin.readLine());
        double maxAcceleration = Double.parseDouble(stdin.readLine());

        int numCars = Integer.parseInt(stdin.readLine());
        int totalTime = Integer.parseInt(stdin.readLine());

        double timestep = Double.parseDouble(stdin.readLine());

        int laneLength = Integer.parseInt(stdin.readLine());

        // Initial printing
        System.out.print(initVelocity + " ");
        System.out.print(maxVelocity + " ");
        System.out.print(maxAcceleration + " ");
        System.out.print(timestep + " ");

        // TODO: Remove
        System.out.println();

        /*
         * Generate cars.
         *
         */

        int maxStep = (int) Math.floor(totalTime / timestep);

        // Choose car timestamps
        Set<Integer> timestamps = new HashSet<>();
        for (int count = 0; count < numCars; ++count) {
            int step = rng.nextInt(maxStep);
            while (!timestamps.add(step))
                step = rng.nextInt(maxStep);
        }

        // The last cars in the lane with their corresponding timestamps
        Map<Direction, Car> laneCars = new EnumMap<>(Direction.class);
        Map<Direction, Integer> laneSteps = new EnumMap<>(Direction.class);

        for (int step = 0; step * timestep < totalTime; ++step) {

            // Check if it is a selected timestamp
            if (!timestamps.contains(step))
                continue;

            /*
             * Choose lane.
             *
             */

            int laneIdx = rng.nextInt(4);
            Direction lane = Direction.values()[laneIdx];

            // If no car in lane
            if (!laneCars.containsKey(lane)) {
                Direction choice = Direction.values()[rng.nextInt(4)];
                Car car = new Car(step * timestep, lane, choice);

                print(car);
                laneCars.put(lane, car);
                laneSteps.put(lane, step);
                continue;
            }

            /* Else check the lane and switch 4 times. If nothing is found, exit the program */

            int count;
            for (count = 0; count < 4; ++count) {

                lane = Direction.values()[laneIdx];

                double timePassed = (step - laneSteps.get(lane)) * timestep;
                double distAhead = initVelocity * timePassed;

                // If the lane is valid break, else switch lane
                if (distAhead > Car.LENGTH)
                    break;
                else
                    laneIdx = (laneIdx + 1) % 4;
            }

            // Lane was not found
            if (count == 4) {
                System.out.println("Couldn't find a lane at step=" + step + ", no car spawned");
                continue;
            }

            /*
             * Create car.
             *
             */

            Direction choice = Direction.values()[rng.nextInt(4)];
            Car car = new Car(step * timestep, lane, choice);

            print(car);
            laneCars.put(lane, car);
            laneSteps.put(lane, step);
        }
    }


    private static void print(Car car) {
        System.out.print(car.timestamp() + " ");
        System.out.print(car.lane() + " ");
        System.out.print(car.choice() + " ");

        // TODO: Remove
        System.out.println();
    }
}

