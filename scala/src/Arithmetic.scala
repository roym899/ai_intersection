
import cafesat.api.FormulaBuilder._
import cafesat.api.Formulas._


/**
  * This object contains utility functions for encoding
  * some arithmetic constraints as boolean ones
  */
object Arithmetic {

  /**
   * Transforms a positive integer in binary form into its integer representation.
   * The `head` element of the input list contains the most
   * significant bit (the list is in big-endian form).
   */
  def binary2int(n: List[Boolean]): Int = {
    val doubles = n.reverse.zipWithIndex.map{ case(b, i) => if (b) Math.pow(2, i) else 0 }
    doubles.foldLeft(0){ case(i, d) => i + d.toInt }
  }


  /**
   * Encodes a positive integer number into base 2.
   * The `head` element of the resulting list contains the most significant
   * bit. This function should not return unnecessary leading zeros.
   */
  def int2binary(n: Int): List[Boolean] = {
    def convert(i: Int): List[Boolean] = if (i == 0) Nil else (i % 2 == 1) :: convert(i / 2)
    if (n == 0) List(false) else convert(n).reverse
  }


  /**
   * This function takes two arguments, both representing positive
   * integers encoded in binary as lists of propositional formulas
   * (true for 1, false for 0). It returns
   * a formula that represents a boolean circuit that constraints
   * `n1` to be less than or equal to `n2`
   */
  def lessEquals(n1: List[Formula], n2: List[Formula]): Formula = {
    def inner(l1: List[Formula], l2: List[Formula]): Formula = {
      require(l1.size == l2.size)
      (l1, l2) match {
        case (x :: Nil, y :: Nil) => !x || (x && y)
        case (x :: xs, y :: ys) => !x && y || (x iff y) && inner(xs, ys)
        case _ => sys.error("Unexpected case")
      }
    }
    val (left, right) = ((n1.reverse zipAll (n2.reverse, False, False)) reverse).unzip
    inner(left, right)
  }

  /**
   * A full adder is a circuit that takes 3 one bit numbers, and returns the
   * result encoded over two bits: (cOut, s)
   */
  def fullAdder(a: Formula, b: Formula, cIn: Formula): (Formula, Formula) = {
    (a && b || cIn && (a xor b), a xor b xor cIn)
  }

  /**
   * This function takes two arguments, both representing positive integers
   * encoded as lists of propositional variables. It returns a pair.
   *
   * The first element of the pair is a `List[Formula]`, and it represents
   * the resulting binary number.
   * The second element is a set of intermediate constraints that are created
   * along the way.
   *
   */
  def adder(n1: List[Formula], n2: List[Formula]): (List[Formula], Set[Formula]) = {
    def adderAcc(n1: List[Formula], n2: List[Formula], constraints: Set[Formula]): (List[Formula], Set[Formula]) = {
      require(n1.size == n2.size)
      (n1, n2) match {
        case (x :: Nil, y :: Nil) => {
          val (headExpr, lastExpr) = fullAdder(x, y, false)
          val head = propVar()
          val last = propVar()
          (head :: last :: Nil, Set(head iff headExpr, last iff lastExpr))
        }
        case (x :: xs, y :: ys) => {
          val (z :: zs, newConstraints) = adderAcc(xs, ys, constraints)
          val (headExpr, secondExpr) = fullAdder(x, y, z)
          val head = propVar()
          val second = propVar()
          (head :: second :: zs, newConstraints + (head iff headExpr) + (second iff secondExpr))
        }
        case _ => sys.error("Unexpected case")
      }
    }
    val (left, right) = ((n1.reverse zipAll (n2.reverse, False, False)) reverse).unzip
    return adderAcc(left, right, Set())
  }

  /**
   * A helper function that creates a less-equals formula
   * taking an integer and a formula as parameters
   */
  def lessEqualsConst(cst: Int, n: List[Formula]): Formula = {
    lessEquals(int2binary(cst), n)
  }

  /**
   * A helper function that creates a less-equals formula
   * taking a formula and an integer as parameters
   */
  def lessEqualsConst(n: List[Formula], cst: Int): Formula = {
    lessEquals(n, int2binary(cst))
  }
  
  /**
   * This function counts the number of positive bits in a number.
   * 
   * It takes a list of formulas, and returns a pair.
   * The first element of the pair is a list of formulas representing the number
   * of ones in `ns`.
   * The second element is a set of additional constraints that have been gathered along
   * the way. Hint: see `adder` for understanding how to use additional constraints
   */
  def countPositiveBits(ns: List[Formula]): (List[Formula], Set[Formula]) = {
    ns.foldLeft((List[Formula](false), Set[Formula]())) { case ((tmpSum, tmpAcc), n) =>
      val (r, c) = adder(tmpSum, List(n))
      (r, tmpAcc ++ c)
    }
  }
}
