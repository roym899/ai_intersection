

object Parser {
  def parseCar(car: String): Car = {
    val split = car.split(' ')
    if (split.length != 3)
      throw new IllegalArgumentException

    val lane = parseDir(split(1))
    val choice = parseDir(split(2))

    new Car(split(0).toDouble, lane, choice)
  }

  def parseDir(dir: String): Direction = {
    dir match {
      case "N" => N
      case "S" => S
      case "W" => W
      case "E" => E
    }
  }
}