// Semantic error: Using undefined variable
import "ifj25" for Ifj
class Program {
    static main() {
        var a 
        a = 10
        var result 
        result = a + undefinedVariable  // ERROR: undefinedVariable not declared
        __a = Ifj.write(result)
        __a = Ifj.write("\n")
    }
}
