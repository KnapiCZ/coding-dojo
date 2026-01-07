// Correct: globals used repeatedly inside expression chain
import "ifj25" for Ifj
class Program {
    static main() {
        __a = 3
        __b = 4
        __c = 5
        var r
        r = (__a + __b) * __c - __a
        __a = Ifj.write(r)
        __a = Ifj.write("\n")
    }
}
