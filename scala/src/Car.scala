/**
  * Created by hawk on 10.10.17.
  */
sealed trait Direction

case object N extends Direction
case object S extends Direction
case object W extends Direction
case object E extends Direction

class Car(val dist: Int, val lane: Direction, val choice: Direction)
