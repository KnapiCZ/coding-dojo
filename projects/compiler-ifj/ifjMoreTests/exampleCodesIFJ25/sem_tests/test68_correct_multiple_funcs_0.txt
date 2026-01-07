// Correct: Multiple function definitions
import "ifj25" for Ifj
class Program {
    static add(a, b) {
        return a + b
    }
    
    static multiply(a, b) {
        return a * b
    }
    
    static square(x) {
        return x*x
    }
    
    static main() {
        var x 
        x = 5
        var sum 
        sum = add(x, 3)
        var sq 
        sq = square(x)
        
        __a = Ifj.write("Sum: ")
        __a = Ifj.write(sum)
        __a = Ifj.write("\n")
        __a = Ifj.write("Square: ")
        __a = Ifj.write(sq)
        __a = Ifj.write("\n")
    }
}
