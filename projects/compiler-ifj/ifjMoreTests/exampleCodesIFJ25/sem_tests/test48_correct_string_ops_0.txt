// Correct: Valid string operations
import "ifj25" for Ifj
class Program {
    static main() {
        var str1 
        str1 = "Hello"
        var str2 
        str2 = "World"
        var result 
        result = str1 + " " + str2
        __a = Ifj.write(result)
        __a = Ifj.write("\n")
        
        var repeated 
        repeated = str1 * 3
        __a = Ifj.write(repeated)
        __a = Ifj.write("\n")
        
        var cmp 
        cmp = Ifj.strcmp(str1, str2)
        __a = Ifj.write("Compare result: ")
        __a = Ifj.write(cmp)
        __a = Ifj.write("\n")
    }
}
