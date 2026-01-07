// Semantic error: String division
import "ifj25" for Ifj
class Program {
    static main() {
        var str 
        str = "hello"
        var num 
        num = 2
        var result 
        result = "str" / num  // ERROR: cannot divide string
        __a = Ifj.write(result)
        __a = Ifj.write("\n")
    }
}
