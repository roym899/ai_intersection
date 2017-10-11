import cafesat.api.Solver.solveForSatisfiability

import cafesat.api.FormulaBuilder._
import cafesat.api.Formulas._

object Main extends App {
  val p = propVar()
  val q = propVar()
  
  val f: Formula = p && !p
  
  solveForSatisfiability(f) match {
    case None => println("Nope!")
    case Some(model) => 
      println("p is " + model.get(p))
      println("q is " + model.get(q))
  }
}
