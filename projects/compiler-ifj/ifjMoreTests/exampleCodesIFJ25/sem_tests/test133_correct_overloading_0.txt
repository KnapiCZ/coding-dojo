// Correct: function overloading with different arity
import "ifj25" for Ifj
class Program {
    static test() {
        return 0
    }
    
    static test(a) {
        return a
    }
    
    static test(a, b) {
        return a + b
    }
    
    static main() {
        var r1
        var r2
        var r3
        r1 = test()
        r2 = test(5)
        r3 = test(3, 4)
        __a = Ifj.write(r1)
        __a = Ifj.write("\n")
        __a = Ifj.write(r2)
        __a = Ifj.write("\n")
        __a = Ifj.write(r3)
        __a = Ifj.write("\n")
    }
}
