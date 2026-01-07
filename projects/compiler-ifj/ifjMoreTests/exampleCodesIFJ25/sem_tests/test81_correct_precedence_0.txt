// Correct: Operator precedence
import "ifj25" for Ifj
class Program {
    static main() {
        var result 
        result = 1 + 2 * 3
        __a = Ifj.write("1+2*3 = ")
        __a = Ifj.write(result)
        __a = Ifj.write("\n")
        
        var result2 
        result2 = (1 + 2) * 3
        __a = Ifj.write("(1+2)*3 = ")
        __a = Ifj.write(result2)
        __a = Ifj.write("\n")
        
        var result3 
        result3 = 10 - 2 * 3 + 4 / 2
        __a = Ifj.write("Complex: ")
        __a = Ifj.write(result3)
        __a = Ifj.write("\n")
    }
}
