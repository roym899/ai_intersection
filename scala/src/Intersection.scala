
sealed trait Cell

case object NW extends Cell
case object NE extends Cell
case object SW extends Cell
case object SE extends Cell

class Slot(val cell: Cell, val step: Int)