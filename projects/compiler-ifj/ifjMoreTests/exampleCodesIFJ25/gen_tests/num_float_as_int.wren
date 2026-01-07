import "ifj25" for Ifj

class Program {
    static main() {
        // 'A' is 65. We use 65.0 (float literal)
        var val
        val = 65.0
        
        var c
        c = Ifj.chr(val)
        Ifj.write("Chr(65.0): ")
        Ifj.write(c)
        Ifj.write("\n")
        
        var s
        s = "Hello World"
        
        // Substring with float indices (0.0, 5.0)
        var start
        start = 0.0
        var end
        end = 5.0
        
        var sub
        sub = Ifj.substring(s, start, end)
        Ifj.write("Substring(0.0, 5.0): ")
        Ifj.write(sub)
        Ifj.write("\n")
        
        // Ord with float index
        var o
        o = Ifj.ord(s, 1.0) // 'e' -> 101
        Ifj.write("Ord('e'): ")
        Ifj.write(o)
        Ifj.write("\n")
        
        // Check if valid comparison works between 5 (int) and 5.0 (float)
        if (5 == 5.0) {
            Ifj.write("5 == 5.0 is true\n")
        } else {
            Ifj.write("5 == 5.0 is false\n")
        }
    }
}