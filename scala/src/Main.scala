import cafesat.api.FormulaBuilder._
import cafesat.api.Formulas._
import cafesat.api.Solver._
import Arithmetic._


/**
 * This is the main method which output the solution in the following format:
 * <Length of each lane>
 * (for each car:)
 * <Lane> <Direction_Wished> <Distance_to_the_intersection> <When_the_car_should_be_at_the_intersection_to_pass>
 */
object Main extends App {
  /**
   * This is a commented example quickly explaining how CafeSat works.
   * We create a simple formula first and then try to solve it with the solver (solveForSatisfiability)
   */
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

  // Tweak these 3 variables to get solutions of what kind of parameters you want to simulate.
  // Be careful about two main things: Critical case (like too few places for too much cars) are not handled
  // Moreover, too much cars or too much timeMax can quickly become slow, SAT is not polynomial.
  // (we were at max with timeMax = 12 and num = 15)
  val timeMax = 11
  val length = 7
  val num = 17
  
  val beg = System.currentTimeMillis()

  val cars = Spawner.spawn(length, num)
  
  /*
   * This loop ensure to get the optimal solution, i.e. the less time to pass all cars.
   */
  val solutions = for(i <- 5 to timeMax) yield { // Ensure that if cars should turn left, they will have time to do at least that
     val propVars = Scheduler.createMap(cars, i)
     findBestSolution(i, propVars)
  }
  
  /**
   * This is the main function. Given all propositional variables and a time, find a solution.
   */

  def findBestSolution(time: Int, propVars: Map[(Car, Slot), PropVar]): Option[List[(Car, Slot)]] = {
     /*
   * STEP 1: Construct a formula for each slots. 
   * These formulas state that no two cars can be on the same slot, i.e. same cell at the same time.
   */

  val groupedBySlot = propVars groupBy { case ((c, s), p) => s }
  val slotsToProps = groupedBySlot map { case (s, m) => (s, m.values) }

  val uniqueConstraint = slotsToProps map {
    case (s, ps) =>
      val stuff = for {
        p <- ps
        q <- ps
        if p != q
      } yield {
        !p || !q
      }
      and(stuff.toSeq: _*)
  }

  /*
   * STEP 2: Create a constraint that ensure that each slot taken by a car to go to its destination should follow.
   * Moreover, they should also follow on the right steps.
   * 
   */

  // Group by cars
  val groupedByCars = propVars groupBy { case ((c, s), p) => c }

  // Each car yield a formula ensuring that it takes cells in the right order and followed in time
  val followConstraint = groupedByCars map {
    case (car, props) => {

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
      // Well, it is really much simpler to explain this with a draw. If you are more interested, you can
      //  ask the question during the presentation
      val followProps = cellProps.foldRight(List.fill(timeMax + 1)(bool2formula(true))) {
        (props, acc) => ((if(acc.size > 0) acc.tail else Nil) zip props) map { case (f1, f2) => f1 && f2 }
      }
      or(followProps: _*)
    }
  }

  /*
   * STEP 3: Counts steps that the car will exactly take to go through the intersection
   * (i.e. 3 steps for turning left, 2 to go straightforward
   * and 1 to turn right)
   * 
   * Note: It seems small, but is actually the hardest. To construct a formula that yields "true"
   * if and only if eg. 3 propositions will be assigned to true exactly requires the use of carry-adders.
   * (see Arithmetic.scala)
   */

  val countConstraint = groupedByCars.map {
    case (c, props) => {
      val count = Util.cells(c.lane, c.choice).size
      val propsForCar = props.unzip._2.toList
      val (positive, extra) = countPositiveBits(propsForCar)
      lessEqualsConst(count, positive) && lessEqualsConst(positive, count) && and(extra.toSeq: _*)
    }
  }

  /*
   * STEP 4: "Behind". Ensure that each car can only go to the intersection
   * if and only if all cars in front on the lane already passed.
   * 
   * The idea is to group proposition variables by lanes, then keep only one Cell per car per time,
   * then for each lane, group again by car and finally sort the data structure by the distance car - intersection
   */

  val groupedByLane = propVars.groupBy { case ((c, s), p) => c.lane }

  val filteredByFirstCell = groupedByLane.map {
    case (l, m) => (l, m.filter {
      case ((c, s), p) => Util.cells(c.lane, c.choice).head == s.cell // Slot must be initial cell for the car
    })
  }

  val groupedByLaneCar = filteredByFirstCell.mapValues {
    props =>
      {
        val grouped = props.groupBy { case ((c, s), p) => c }
        grouped
      }
  }

  val groupedByLaneOrderedByCar = groupedByLaneCar.mapValues {
    case dick => dick.toList.sortBy {
      case (c, props) => c.dist
    }
  }

  // Now that we have a nice data structure, we can check, two cars by two, if one behind never goes before the other in front.
  val behindConstraint = groupedByLaneOrderedByCar.map {
    case (lane, cars) => {
      val isFollowedCorrectly = for (t <- cars.sliding(2)) yield {
        // it means that the car is alone or no cars are on this lane, which is not a problem and therefore not a constraint.
        if (t.size != 2)
          bool2formula(true)
        else {
          val prevProps = t(0)._2
          val currProps = t(1)._2
          val res = for (curr <- currProps) yield {
            val ors = for (prev <- prevProps if (prev._1._2.step < curr._1._2.step)) yield (curr._2 && prev._2)
            or(ors.toSeq: _*)
          }
          or(res.toSeq: _*)
        }
      }
      and(isFollowedCorrectly.toSeq: _*)
    }
  }

  /*
     * Try the model by putting all constraint together and link them by the
     * and logical operator.
     * 
     */

  val allConstraints = uniqueConstraint ++ followConstraint ++ countConstraint ++ behindConstraint

  // Here is the call of the solver.
  val solving = solveForSatisfiability(and(allConstraints.toSeq: _*))

  // We now have to interpret the result, i.e. find which proposition variables are finally turned into "true"
  val result = solving map { model =>
    val temp = propVars.filter {
      case ((c, s), p) => model(p)
    }.toList.map {
      case ((c, s), p) => (c, s)
    }
    temp.sortBy { case (c, s) => s.step }
  }
  // output the final result
  result
  }

 /*
  * We kept this in case of eventually other bugs.
  * This will print the cars before eventually no solutions are found
  * and then print more informations
  */
  //println("CARS")
  //cars.foreach(c => println(c.lane, c.choice, c.dist))
  //println("RES")
  //solutions.dropWhile(p => !p.isDefined).head.get.foreach { case (c, s) => println(c.lane, c.choice, c.dist, s.cell, s.step) }
  
  
  /*
   * Here is the output in the right format explained at the begining of this document.
   * To find the best solution, find the first time that is enough to output a solution
   */
  val bestSolution = solutions.dropWhile(p => !p.isDefined).head.get
  // keep only information about the first cell used by each car in the intersection.
  val firstCells = bestSolution.groupBy{ case (c, s) => c }.map {case (c, l) => l.minBy{case (car, slot) => slot.step}}
  println(length)
  firstCells.map { case (c, s) => println(c.lane + " " +  c.choice + " " +  c.dist + " " + s.step) }
  val end = System.currentTimeMillis()
  println( (end - beg))
}


