import cafesat.api.Formulas._
import cafesat.api.FormulaBuilder._

object Scheduler {
  def createMap(cars: List[Car], steps: Int): Map[(Car, Slot), PropVar] = {
    val allCells = NW :: NE :: SW :: SE :: Nil
    val allSlots = ((0 to steps).toList) flatMap (s => allCells.map(new Slot(_, s)))
    
    val allKeys = cars flatMap { c =>
      allSlots filter (s => Util.cells(c.lane, c.choice).contains(s.cell) && s.step > c.dist) map ((c, _))
    }
    
    return allKeys.map(k => (k, propVar())).toMap
  }
}

object Util {
  def cells(lane: Direction, choice: Direction): List[Cell] = lane match {
    case N => choice match {
      case N => Nil
      case W => List(NW)
      case S => List(NW, SW)
      case E => List(NW, SW, SE)
    }
    case S => choice match {
      case S => Nil
      case E => List(SE)
      case N => List(SE, NE)
      case W => List(SE, NE, NW)
    }
    case W => choice match {
      case W => Nil
      case S => List(SW)
      case E => List(SW, SE)
      case N => List(SW, SE, NE)
    }
    case E => choice match {
      case E => Nil
      case N => List(NE)
      case W => List(NE, NW)
      case S => List(NE, NW, SW)
    }
  }
}