// Semantic error: Variable redefinition in same scope
import "ifj25" for Ifj
class Program {
    static main() {
        var x 
        x = 5
        var x  // ERROR: x already defined in this scope
        x = 10
        Ifj.write(x)
        Ifj.write("\n")
    }
}
