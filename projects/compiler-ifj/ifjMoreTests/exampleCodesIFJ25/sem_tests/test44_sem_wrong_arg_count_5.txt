// Semantic error: Wrong number of arguments
import "ifj25" for Ifj
class Program {
    static add(a, b) {
        return a + b
    }
    
    static main() {
        var result 
        result = add(5)  // ERROR: add expects 2 arguments, got 1
        __a = Ifj.write(result)
        __a = Ifj.write("\n")
    }
}
