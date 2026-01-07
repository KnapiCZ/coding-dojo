// Correct: pass globals as arguments to user function
import "ifj25" for Ifj
class Program {
    static sum(a, b) {
        var r
        r = a + b
        return r
    }
    static main() {
        __gA = 6
        __gB = 9
        var res
        res = sum(__gA, __gB)
        __a = Ifj.write(res)
        __a = Ifj.write("\n")
    }
}
