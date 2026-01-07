// Semantic error: Function redefinition
import "ifj25" for Ifj
class Program {
    static calculate(x) {
        return x * 2
    }
    
    static calculate(x) {  // ERROR: function already defined
        return x * 3
    }
    
    static main() {
        var result 
        result = calculate(5)
        Ifj.write(result)
        Ifj.write("\n")
    }
}
