// Correct: globals in if-else branches
import "ifj25" for Ifj
class Program {
    static main() {
        __cond = 0
        __v1 = 7
        __v2 = 9
        if (__cond) {
            __a = Ifj.write(__v1)
            __a = Ifj.write("\n")
        } else {
            __a = Ifj.write(__v2)
            __a = Ifj.write("\n")
        }
    }
}
