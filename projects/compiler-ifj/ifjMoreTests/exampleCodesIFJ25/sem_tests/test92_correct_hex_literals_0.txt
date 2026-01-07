// Correct: Hexadecimal literals
import "ifj25" for Ifj
class Program {
    static main() {
        var hex1 
        hex1 = 0xFF
        __a = Ifj.write("0xFF = ")
        __a = Ifj.write(hex1)
        __a = Ifj.write("\n")
        
        var hex2 
        hex2 = 0xABCD
        __a = Ifj.write("0xABCD = ")
        __a = Ifj.write(hex2)
        __a = Ifj.write("\n")
        
        var result 
        result = 0x10 + 0x20
        __a = Ifj.write("0x10 + 0x20 = ")
        __a = Ifj.write(result)
        __a = Ifj.write("\n")
    }
}
