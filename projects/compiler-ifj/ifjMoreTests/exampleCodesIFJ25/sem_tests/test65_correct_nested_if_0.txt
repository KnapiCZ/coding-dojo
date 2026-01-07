// Correct: Nested if-else statements
import "ifj25" for Ifj
class Program {
    static main() {
        var x 
        x = 15
        
        if (x > 10) {
            __a = Ifj.write("x is greater than 10\n")
            if (x > 20) {
                __a = Ifj.write("x is greater than 20\n")
            } else {
                __a = Ifj.write("x is between 10 and 20\n")
            }
        } else {
            __a = Ifj.write("x is 10 or less\n")
        }
    }
}
