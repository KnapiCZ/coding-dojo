// Correct: Deeply nested parentheses
import "ifj25" for Ifj
class Program {
    static main() {
        var result 
        result = ((((((5))))))
        __a = Ifj.write("Nested result: ")
        __a = Ifj.write(result)
        __a = Ifj.write("\n")
        
        var calc 
        calc = (((1 + 2) * (3 + 4)))
        __a = Ifj.write("Calculation: ")
        __a = Ifj.write(calc)
        __a = Ifj.write("\n")
    }
}
