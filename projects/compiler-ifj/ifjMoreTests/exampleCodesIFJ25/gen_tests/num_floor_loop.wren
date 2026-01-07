import "ifj25" for Ifj

class Program {
    static main() {
        var n
        n = 5.9
        
        var floored
        floored = Ifj.floor(n)
        
        Ifj.write("Original: ")
        Ifj.write(n)
        Ifj.write("\n")
        
        Ifj.write("Floored: ")
        Ifj.write(floored)
        Ifj.write("\n")
        
        // Use floored value in a loop (must be int logic)
        Ifj.write("Counting: ")
        for (i in 1 .. floored) {
            Ifj.write(i)
            Ifj.write(" ")
        }
        Ifj.write("\n")
        
        // Type Check
        var intVal
        intVal = 10
        var floatVal
        floatVal = 10.0
        
        // Using standalone calls (EXTFUN)
        Ifj.write("Types match? ")
        if (intVal == floatVal) {
            Ifj.write("Yes")
        } else {
            Ifj.write("No")
        }
        Ifj.write("\n")
    }
}