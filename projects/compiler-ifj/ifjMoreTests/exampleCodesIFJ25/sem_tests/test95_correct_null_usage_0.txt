// Correct: Null literal usage
import "ifj25" for Ifj
class Program {
    static main() {
        var x 
        x = null
        
        if (x == null) {
            __a = Ifj.write("x is null\n")
        } else {
            __a = Ifj.write("x is not null\n")
        }
        
        if (x != null) {
            __a = Ifj.write("x has value\n")
        } else {
            __a = Ifj.write("x is still null\n")
        }
        
        var y 
        y = 10
        
        if (y != null) {
            __a = Ifj.write("y has value: ")
            __a = Ifj.write(y)
            __a = Ifj.write("\n")
        } else {
            __a = Ifj.write("y is null\n")
        }
    }
}
