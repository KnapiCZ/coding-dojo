// Semantic error: using undefined local variable
import "ifj25" for Ifj
class Program {
    static main() {
        var x
        x = y + 5  // y is not defined
        __a = Ifj.write(x)
        __a = Ifj.write("\n")
    }
}
