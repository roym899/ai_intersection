

object Main extends App {
  val input = io.Source.stdin.getLines().toList

  val initial = input.head
  val cars = input.tail

  val parsed = cars.map(car => Parser.parseCar(car))
  parsed.foreach(println)
}
