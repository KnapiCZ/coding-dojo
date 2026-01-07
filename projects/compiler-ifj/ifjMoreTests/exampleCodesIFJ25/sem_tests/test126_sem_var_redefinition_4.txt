// Semantic error: variable redefinition in same scope
import "ifj25" for Ifj
class Program {
    static main() {
        var x
        x = 10
        var x  // redefinition of x in same scope
        x = 20
        Ifj.write(x)
        Ifj.write("\n")
    }
}
