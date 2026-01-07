// Correct: String concatenation and multiplication
import "ifj25" for Ifj
class Program {
    static main() {
        var str1 
        str1 = "Hello"
        var str2 
        str2 = "World"
        
        var greeting 
        greeting = str1 + " " + str2
        __a = Ifj.write(greeting)
        __a = Ifj.write("\n")
        
        var repeated 
        repeated = "Ha" * 3
        __a = Ifj.write(repeated)
        __a = Ifj.write("\n")
        
        var multi 
        multi = (str1 * 2) + (str2 * 2)
        __a = Ifj.write(multi)
        __a = Ifj.write("\n")
    }
}
