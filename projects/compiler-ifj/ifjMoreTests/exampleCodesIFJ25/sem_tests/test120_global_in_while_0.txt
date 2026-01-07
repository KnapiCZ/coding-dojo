// Correct: globals in while loop controlling and updating
import "ifj25" for Ifj
class Program {
    static main() {
        __i = 0
        __limit = 0
        while (__i == __limit) {
            __a = Ifj.write(__i)
            __a = Ifj.write("\n")
            __i = __i + 1
        }
    }
}
