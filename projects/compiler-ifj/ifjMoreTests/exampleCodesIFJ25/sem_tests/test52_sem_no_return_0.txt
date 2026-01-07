// Semantic error: Function missing return statement (if required)
import "ifj25" for Ifj
class Program {
    static calculate(x) {
        var result 
        result = x * 2
        // ERROR: missing return statement
    }
    
    static main() {
        var val 
        val = calculate(5)
        __a = Ifj.write(val)
        __a = Ifj.write("\n")
    }
}
