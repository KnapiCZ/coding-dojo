// Correct: While loop with counter
import "ifj25" for Ifj
class Program {
    static main() {
        var counter 
        counter = 0
        var sum 
        sum = 0
        
        while (counter < 5) {
            sum = sum + counter
            __a = Ifj.write("Counter: ")
            __a = Ifj.write(counter)
            __a = Ifj.write(", Sum: ")
            __a = Ifj.write(sum)
            __a = Ifj.write("\n")
            counter = counter + 1
        }
        
        __a = Ifj.write("Final sum: ")
        __a = Ifj.write(sum)
        __a = Ifj.write("\n")
    }
}
