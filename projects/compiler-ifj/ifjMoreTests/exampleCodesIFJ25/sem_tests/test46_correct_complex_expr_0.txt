// Correct: Complex arithmetic expressions
import "ifj25" for Ifj
class Program {
    static main() {
        var a 
        a = 10
        var b 
        b = 5
        var c 
        c = 2
        var result 
        result = (a + b) * c - a / b
        __a = Ifj.write("Result: ")
        __a = Ifj.write(result)
        __a = Ifj.write("\n")
        
        var d 
        d = a > b
        if (d) {
            __a = Ifj.write("a is greater than b\n")
        } else {
            __a = Ifj.write("a is not greater than b\n")
        }
    }
}
