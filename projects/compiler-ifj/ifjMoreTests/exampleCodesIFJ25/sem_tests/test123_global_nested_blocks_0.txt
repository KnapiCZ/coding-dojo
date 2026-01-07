// Correct: globals accessible in nested blocks
import "ifj25" for Ifj
class Program {
    static main() {
        __base = 2
        {
            __mul = 3
            var r1
            r1 = __base * __mul
            __a = Ifj.write(r1)
            __a = Ifj.write("\n")
            {
                __add = 4
                var r2
                r2 = (__base + __add) * __mul
                __a = Ifj.write(r2)
                __a = Ifj.write("\n")
            }
        }
    }
}
