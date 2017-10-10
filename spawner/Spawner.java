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
     * TODO: Choice must not be the same lane that a car comes from.
     *
     * @param args
     * @throws IOException
     */
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
        System.out.print(laneLength + " ");
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

        for (int step = 0; step * timestep < totalTime; ++step) {

            // Check if it is a selected timestamp
            if (!timestamps.contains(step))
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
                System.out.println("Couldn't find a lane at step=" + step + ", no car spawned");
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
    }


    private static void print(Car car) {
        System.out.print(car.timestamp() + " ");
        System.out.print(car.lane() + " ");
        System.out.print(car.choice() + " ");

        // TODO: Remove
        System.out.println();
    }
}

