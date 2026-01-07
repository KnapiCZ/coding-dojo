// Semantic error: parameter name used twice in function signature
import "ifj25" for Ifj
class Program {
    static duplicate(x, x) {  // x defined twice
        return x + x
    }
    
    static main() {
        var result
        result = duplicate(5, 10)
        Ifj.write(result)
        Ifj.write("\n")
    }
}
