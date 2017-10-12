import cafesat.api.FormulaBuilder._
import cafesat.api.Formulas._
import cafesat.api.Solver._
import Arithmetic._

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
    
  val time = 5
  val length = 5
  val num = 2
  
  val cars = Spawner.spawn(length, num)
  val propVars = Scheduler.createMap(cars, time)
  
  /*
   * STEP 1: Unique
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
   * STEP 2: Follow
   * 
   */

  // Group by cars
  val groupedByCars  = propVars groupBy { case ((c, s), p) => c}
  
  // Group
  val followConstraint = groupedByCars map { case (car, props) => {
    
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
      
      val followProps = cellProps.foldRight(List.fill(time + 1)(bool2formula(true))){
        (props, acc) => (acc.tail zip props) map { case (f1, f2) => f1 && f2 }
      }
      
      or(followProps:_*)
    }
  }
  
  /*
   * STEP 3: Count 
   * 
   */
  
  val countConstraint = groupedByCars.map {
    case (c, props) => {
      val count = Util.cells(c.lane, c.choice).size
      val propsForCar = props.unzip._2.toList
      val (positive, extra) = countPositiveBits(propsForCar)
      lessEqualsConst(count, positive) && lessEqualsConst(positive, count) && and(extra.toSeq:_*)
    }
  }
  
  /*
   * STEP 4: Behind
   * 
   */
  
  val groupedByLane = propVars.groupBy { case ((c, s), p) => c.lane }
  
  val filteredByFirstCell = groupedByLane.map {
    case (l, m) => (l, m.filter { 
      case ((c, s), p) => Util.cells(c.lane, c.choice).head == s.cell // Slot must be initial cell for the car
    })
  }
  
  val groupedByLaneCar = filteredByFirstCell.mapValues {
    props => {
      val grouped = props.groupBy { case ((c, s), p) => c }
      //val ordered = grouped.map {
      //  case (car, props2) => (car, props2.toList.sortBy { case ((_, s), _) => s.step })
      //}
      grouped
    }
  }
  
  val groupedByLaneOrderedByCar = groupedByLaneCar.mapValues {
    case dick => dick.toList.sortBy {
      case (c, props) => c.dist
    }
  }
  
  val behindConstraint = groupedByLaneOrderedByCar.map {
    case (lane, cars) => {
      val tits = for (t <- cars.sliding(2)) yield {
        if (t.size != 2)
          bool2formula(true)
        else {
          val prevProps = t(0)._2
          val currProps = t(1)._2
          val res = for (curr <- currProps) yield {
            val ors = for (prev <- prevProps if (prev._1._2.step < curr._1._2.step)) yield (curr._2 && prev._2)
            or(ors.toSeq:_*)
          }
          and(res.toSeq:_*)
          }
        }
        and(tits.toSeq:_*)
    }
  }
    
    /*
     * FINAL: Fuck this
     * 
     */
    
  val allConstraints = uniqueConstraint ++ followConstraint ++ countConstraint ++ behindConstraint
  
  val result = solveForSatisfiability(and(allConstraints.toSeq:_*))
  
  val suckmydickd = result.map { model =>
    propVars.filter {
      case ((c, s), p) => model(p)
    }.map {
      case ((c, s), p) => (c, s)
    }.toList
  }
  
  suckmydickd.get.foreach { case (c, s) => println(c.lane, c.choice, c.dist, s.cell, s.step) }
}
