// Correct: basic global integer arithmetic
import "ifj25" for Ifj
class Program {
    static main() {
        __g1 = 10
        var x
        x = __g1 + 5
        __a = Ifj.write(x)
        __a = Ifj.write("\n")
    }
}
