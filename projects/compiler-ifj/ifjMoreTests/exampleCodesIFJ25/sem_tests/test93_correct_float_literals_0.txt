// Correct: Float and exponential notation
import "ifj25" for Ifj
class Program {
    static main() {
        var pi 
        pi = 3.14159
        __a = Ifj.write("Pi: ")
        __a = Ifj.write(pi)
        __a = Ifj.write("\n")
        
        var e 
        e = 2.71828
        __a = Ifj.write("e: ")
        __a = Ifj.write(e)
        __a = Ifj.write("\n")
        
        var sci 
        sci = 1.5e3
        __a = Ifj.write("1.5e3 = ")
        __a = Ifj.write(sci)
        __a = Ifj.write("\n")
        
        var small 
        small = 2.5e-2
        __a = Ifj.write("2.5e-2 = ")
        __a = Ifj.write(small)
        __a = Ifj.write("\n")
    }
}
