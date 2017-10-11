

object Parser {
  def parseDir(dir: String): Direction = dir match {
    case "N" => N
    case "S" => S
    case "W" => W
    case "E" => E
  }
}