
/**
 * Express the 4 cells representing together the intersection.
 */
sealed trait Cell

case object NW extends Cell
case object NE extends Cell
case object SW extends Cell
case object SE extends Cell

/**
 * Express what a Slot is: a certain cell on a certain time
 */
class Slot(val cell: Cell, val step: Int)