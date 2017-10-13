import scala.util.Random

/**
 * This object will spawn cars on random lanes and random distances.
 */
object Spawner {
  def spawn(length: Int, cars: Int): List[Car] = {
    val rng = new Random(42)
    
    def gen() = rng.nextInt(4) match {
      case 0 => N
      case 1 => S
      case 2 => W
      case 3 => E
    }
    
    def rec(n: Int, acc: List[Car]): List[Car] = {
      if (n == 0)
        acc
      else {
        val dist = rng.nextInt(length)
        val lane = gen()
        val choice = gen()
        // ensure that the place is free and that the lane is not the same as the choice
        if (acc.forall(c => c.lane != lane || c.dist != dist) && lane != choice)
          rec(n - 1, new Car(dist, lane, choice) :: acc)
        else
          rec(n, acc)
      }
    }
    
    rec(cars, Nil)
  }
}