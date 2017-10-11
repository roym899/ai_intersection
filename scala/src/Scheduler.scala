import cafesat.api.Formulas._
import cafesat.api.FormulaBuilder._

object Scheduler {
  def createMap(cars: List[Car], steps: Int): Map[(Car, Slot), PropVar] = {
    val allCells = NW :: NE :: SW :: SE :: Nil
    val allSlots = ((0 to steps).toList) flatMap (s => allCells.map(new Slot(_, s)))
    val allKeys = cars flatMap { c =>
      allSlots filter (s => Util.cells(c.lane, c.choice).contains(s.cell)) map (s => (c, s))
    }
    return allKeys.map(k => (k, propVar())).toMap
  }
}

object Util {
  def cells(lane: Direction, choice: Direction): Set[Cell] = lane match {
    case N => choice match {
      case N => Set.empty
      case W => Set(NW)
      case S => Set(NW, SW)
      case E => Set(NW, SW, SE)
    }
    case S => choice match {
      case S => Set.empty
      case E => Set(SE)
      case N => Set(SE, NE)
      case W => Set(SE, NE, NW)
    }
    case W => choice match {
      case W => Set.empty
      case S => Set(SW)
      case E => Set(SW, SE)
      case N => Set(SW, SE, NE)
    }
    case E => choice match {
      case E => Set.empty
      case N => Set(NE)
      case W => Set(NE, NW)
      case S => Set(NE, NW, SW)
    }
  }
}