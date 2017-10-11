/**
 * Created by hawk on 09.10.17.
 */
public class Car {

    public static final int LENGTH = 4;

    private double timestamp;
    private final Direction lane, choice;

    public Car(double timestamp, Direction lane, Direction choice) {
        if (lane == choice)
            throw new IllegalArgumentException("Choice cannot be the same as lane");

        this.timestamp = timestamp;
        this.lane = lane;
        this.choice = choice;
    }

    public double timestamp() {
        return timestamp;
    }

    public Direction lane() {
        return lane;
    }

    public Direction choice() {
        return choice;
    }
}
