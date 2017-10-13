/**
  * Express what a direction is.
  */
sealed trait Direction

case object N extends Direction
case object S extends Direction
case object W extends Direction
case object E extends Direction

/**
 * A car is on a certain distance of the intersection, on a certain lane and wants to go on a certain direction.
 */
class Car(val dist: Int, val lane: Direction, val choice: Direction)
