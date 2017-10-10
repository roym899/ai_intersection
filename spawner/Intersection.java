import java.util.EnumMap;
import java.util.Map;

/**
 * Created by hawk on 09.10.17.
 */
public class Intersection {
    public static enum Cell {
        NE, NW, SE, SW;
    }

    public final Map<Cell, Integer> timesLeft = new EnumMap<>(Cell.class);
}
