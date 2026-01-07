// Correct: global strings in concatenation and repetition
import "ifj25" for Ifj
class Program {
    static main() {
        __s1 = "Hi"
        __s2 = "There"
        var out
        out = (__s1 + " ") + __s2
        __a = Ifj.write(out)
        __a = Ifj.write("\n")
        var rep
        rep = __s1 * 3
        __a = Ifj.write(rep)
        __a = Ifj.write("\n")
    }
}
