// Correct: Comparison operators
import "ifj25" for Ifj
class Program {
    static main() {
        var a 
        a = 5
        var b 
        b = 10
        
        if (a < b) {
            __a = Ifj.write("a is less than b\n")
        } else {
            __a = Ifj.write("a is not less\n")
        }
        
        if (a <= b) {
            __a = Ifj.write("a is less or equal\n")
        } else {
            __a = Ifj.write("a is greater\n")
        }
        
        if (a == a) {
            __a = Ifj.write("a equals itself\n")
        } else {
            __a = Ifj.write("impossible\n")
        }
    }
}
