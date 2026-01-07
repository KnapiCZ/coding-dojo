import "ifj25" for Ifj

class Program {
    static countdown(n) {
        if (n < 0) {
            return
        }
        
        Ifj.write(n)
        Ifj.write(" ")
        
        // Recursive call without assignment (EXTFUN)
        countdown(n - 1)
        
        // Post-recursion print (unwinding stack)
        if (n == 0) {
            Ifj.write("LIFT OFF!\n")
        }
    }

    static main() {
        Ifj.write("T-Minus: ")
        
        // Standalone call
        countdown(5)
    }
}