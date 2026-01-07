// Correct: Variable shadowing in nested scope
import "ifj25" for Ifj
class Program {
    static main() {
        var x 
        x = 5
        __a = Ifj.write("Outer x: ")
        __a = Ifj.write(x)
        __a = Ifj.write("\n")
        {
            var x 
            x = 10
            __a = Ifj.write("Inner x: ")
            __a = Ifj.write(x)
            __a = Ifj.write("\n")
        }
        __a = Ifj.write("Outer x again: ")
        __a = Ifj.write(x)
        __a = Ifj.write("\n")
    }
}
