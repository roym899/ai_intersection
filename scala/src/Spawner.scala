import scala.util.Random


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
        if (acc.forall(c => c.lane != lane || c.dist != dist) && lane != choice)
          rec(n - 1, new Car(dist, lane, choice) :: acc)
        else
          rec(n, acc)
      }
    }
    
    rec(cars, Nil)
  }
}