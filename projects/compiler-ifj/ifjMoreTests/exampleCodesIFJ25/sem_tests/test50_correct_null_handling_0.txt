// Correct: Null handling and type checking
import "ifj25" for Ifj
class Program {
    static main() {
        var x 
        x = Ifj.read_num()
        if (x != null) {
            if (x is Num) {
                __a = Ifj.write("Got a number: ")
                __a = Ifj.write(x)
                __a = Ifj.write("\n")
            } else {
                __a = Ifj.write("Not a number\n")
            }
        } else {
            __a = Ifj.write("Input was null\n")
        }
        
        var y 
        y = null
        if (y is Null) {
            __a = Ifj.write("y is null\n")
        } else {
            __a = Ifj.write("y is not null\n")
        }
    }
}
