import "ifj25" for Ifj

class Program {
    static main() {
        var i
        i = 10
        var f
        f = 2.5
        
        var res
        
        // Int + Float -> Float
        res = i + f
        Ifj.write("10 + 2.5 = ")
        Ifj.write(res)
        Ifj.write("\n")
        
        // Float - Int -> Float
        res = f - 1
        Ifj.write("2.5 - 1 = ")
        Ifj.write(res)
        Ifj.write("\n")
        
        // Int / Int -> Float (Always)
        res = 7 / 2
        Ifj.write("7 / 2 = ")
        Ifj.write(res)
        Ifj.write("\n")
        
        // Exact division checking
        res = 10 / 2
        Ifj.write("10 / 2 = ")
        Ifj.write(res) // Should be 5.0 (float)
        Ifj.write("\n")
        
        // Multiplication
        res = i * f
        Ifj.write("10 * 2.5 = ")
        Ifj.write(res)
        Ifj.write("\n")
    }
}