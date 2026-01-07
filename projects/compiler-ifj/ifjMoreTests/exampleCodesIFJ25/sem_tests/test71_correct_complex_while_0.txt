// Correct: Complex while loop with nested blocks
import "ifj25" for Ifj
class Program {
    static main() {
        var i 
        i = 0
        var sum 
        sum = 0
        
        while (i < 10) {
            var temp 
            temp = i * 2
            
            if (temp > 10) {
                sum = sum + temp
            } else {
                sum = sum + i
            }
            
            __a = Ifj.write("i: ")
            __a = Ifj.write(i)
            __a = Ifj.write(", sum: ")
            __a = Ifj.write(sum)
            __a = Ifj.write("\n")
            
            i = i + 1
        }
        
        __a = Ifj.write("Final sum: ")
        __a = Ifj.write(sum)
        __a = Ifj.write("\n")
    }
}
