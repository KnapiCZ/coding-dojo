// Correct: Type checking with is operator
import "ifj25" for Ifj
class Program {
    static main() {
        var x 
        x = 42
        
        if (x is Num) {
            __a = Ifj.write("x is a number\n")
        } else {
            __a = Ifj.write("x is not a number\n")
        }
        
        var str 
        str = "hello"
        
        if (str is String) {
            __a = Ifj.write("str is a string\n")
        } else {
            __a = Ifj.write("str is not a string\n")
        }
        
        var n 
        n = null
        
        if (n is Null) {
            __a = Ifj.write("n is null\n")
        } else {
            __a = Ifj.write("n is not null\n")
        }
    }
}
