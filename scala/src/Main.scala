import cafesat.api.FormulaBuilder._
import cafesat.api.Formulas._

object Main extends App {
//  val p = propVar()
//  val q = propVar()
//  
//  val f: Formula = p && !q || q
//  
//  solveForSatisfiability(f) match {
//    case None => println("Nope!")
//    case Some(model) => 
//      println("p is " + model.get(p))
//      println("q is " + model.get(q))
//  }
    
  val time = 100
  val length = 10
  val num = 10
  
  val cars = Spawner.spawn(length, num)
  val propVars = Scheduler.createMap(cars, time)
  
  /*
   * STEP 1: All slots must be unique.
   * 
   */
  
  val groupedBySlot = propVars groupBy { case ((c, s), p) => s }
  val slotsToProps = groupedBySlot map { case (s, m) => (s, m.values) }
  
  val uniqueConstraint = slotsToProps map { case (s, ps) => 
    val stuff = for {
      p <- ps
      q <- ps
      if p != q
    } yield {
      !p || !q
    }
    and(stuff.toSeq:_*)
  }
  
  /*
   * STEP 2: Direction slot follows in time
   * 
   */

  // Group by cars
  val groupedByCars  = propVars groupBy { case ((c, s), p) => c}
  
  // Group
  val constraintmaybe = groupedByCars map { case (car, props) => {
    
      // For each car
    
      val groupedByCell = props groupBy {
        case ((_, s), _) => s.cell
      }
      
      val orderedByTime = groupedByCell map {
        case (cell, m) => (cell, m.toList.sortBy { case ((_, s), _) => s.step })
      }
      
      val orderedByCell = for (cell <- Util.cells(car.lane, car.choice)) yield (cell, orderedByTime(cell))
      
      val cellProps = orderedByCell map {
        case (cell, l) => l.unzip._2
      }
      
      val beautiful = cellProps.foldRight(List.fill(time + 1)(bool2formula(true))){
        (props, acc) => (acc.tail zip props) map { case (f1, f2) => f1 && f2 }
      }
      
      or(beautiful:_*)
    }
  }
}
