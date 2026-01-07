// Semantic error: Calling undefined function
import "ifj25" for Ifj
class Program {
    static main() {
        var result 
        result = nonExistentFunction(5)  // ERROR: function not defined
        __a = Ifj.write(result)
        __a = Ifj.write("\n")
    }
}
