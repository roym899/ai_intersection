/**
 * Created by hawk on 09.10.17.
 */
public enum Direction {

    N, S, W, E;

    private static final String[] strings = {"N", "S", "W", "E"};

    @Override
    public String toString() {
        return strings[this.ordinal()];
    }
}
