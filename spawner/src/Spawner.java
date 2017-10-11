import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.*;

/**
 * Created by hawk on 09.10.17.
 */
public class Spawner {

    /**
     * Runs a spawner to generate random cars at random timestamps.
     *
     * @param args
     * @throws IOException
     */
    public static void main(String[] args) throws IOException {
        BufferedReader stdin = new BufferedReader(new InputStreamReader(System.in));
        Random rng = new Random(42);

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
        System.out.print(laneLength + " ");
        System.out.print(timestep + " ");

        System.out.println();

        /*
         * Generate cars.
         *
         */

        int maxStep = (int) Math.floor(totalTime / timestep);

        // Choose car timestamps
        Set<Integer> spawnSteps = new HashSet<>();
        for (int count = 0; count < numCars; ++count) {
            int step = rng.nextInt(maxStep);
            while (!spawnSteps.add(step) && spawnSteps.size() < maxStep)
                step = rng.nextInt(maxStep);
        }


        if (spawnSteps.size() < numCars)
            System.err.println("Too many cars, " + maxStep + " steps available but " + numCars + " have to be spawned");

        // The last cars in the lane with their corresponding timestamps
        Map<Direction, Car> laneCars = new EnumMap<>(Direction.class);

        // Count of cars that couldn't be spawned
        int ditched = 0;

        for (int step = 0; step * timestep < totalTime; ++step) {

            // Check if it is a selected timestamp
            if (!spawnSteps.contains(step))
                continue;

            /*
             * Choose lane.
             *
             */

            // Initial values for lane
            int laneIdx = rng.nextInt(4);
            Direction lane = Direction.values()[laneIdx];

            int count;
            for (count = 0; count < 4; ++count) {

                // If no car in lane
                if (!laneCars.containsKey(lane))
                    break;

                double timePassed = step * timestep - laneCars.get(lane).timestamp();
                double distAhead = initVelocity * timePassed;

                // If the car before is far enough, lane is valid
                if (distAhead > Car.LENGTH)
                    break;

                laneIdx = (laneIdx + 1) % 4;
                lane = Direction.values()[laneIdx];
            }

            // Lane was not found
            if (count == 4) {
                System.err.println("Couldn't find a lane for car at timestamp " + (step * timestep));
                ++ditched;
                continue;
            }

            /*
             * Create car.
             *
             */

            int shift = 1 + rng.nextInt(3); // 1, 2, 3
            int choiceIdx = (lane.ordinal() + shift) % 4;

            Car car = new Car(step * timestep, lane, Direction.values()[choiceIdx]);

            print(car);
            laneCars.put(lane, car);
        }

        System.err.println("Spawning finished. " + ditched + " cars ditched.");
    }


    private static void print(Car car) {
        System.out.print(car.timestamp() + " ");
        System.out.print(car.lane() + " ");
        System.out.print(car.choice() + " ");

        System.out.println();
    }
}

