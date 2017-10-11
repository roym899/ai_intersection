/**
  * Created by hawk on 10.10.17.
  */
sealed trait Direction

case object N extends Direction { val name = "N" }
case object S extends Direction { val name = "S" }
case object W extends Direction { val name = "W" }
case object E extends Direction { val name = "E" }

class Car(val timestamp: Double, val lane: Direction, val choice: Direction)
