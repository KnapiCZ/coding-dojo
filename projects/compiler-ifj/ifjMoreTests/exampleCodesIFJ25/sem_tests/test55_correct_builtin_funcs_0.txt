// Correct: Using all builtin functions
import "ifj25" for Ifj
class Program {
    static main() {
        var num 
        num = 42.7
        var floored 
        floored = Ifj.floor(num)
        __a = Ifj.write("Floor of ")
        __a = Ifj.write(num)
        __a = Ifj.write(" is ")
        __a = Ifj.write(floored)
        __a = Ifj.write("\n")
        
        var str 
        str = Ifj.str(num)
        var len 
        len = Ifj.length(str)
        __a = Ifj.write("String length: ")
        __a = Ifj.write(len)
        __a = Ifj.write("\n")
        
        var sub 
        sub = Ifj.substring(str, 0, 2)
        __a = Ifj.write("Substring: ")
        __a = Ifj.write(sub)
        __a = Ifj.write("\n")
        
        var ascii 
        ascii = Ifj.ord("A", 0)
        __a = Ifj.write("ASCII of A: ")
        __a = Ifj.write(ascii)
        __a = Ifj.write("\n")
        
        var ch 
        ch = Ifj.chr(65)
        __a = Ifj.write("Character 65: ")
        __a = Ifj.write(ch)
        __a = Ifj.write("\n")
    }
}
