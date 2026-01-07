import "ifj25" for Ifj

class Program {
    static factorial(n) {
        if (n < 2) {
            return 1
        } else {
            var prev
            var next
            next = n - 1
            prev = factorial(next)
            
            // Update global call counter
            __calls = __calls + 1
            
            var res
            res = n * prev
            return res
        }
    }

    static main() {
        // Initialize global
        __calls = 0
        
        var n
        n = 5
        
        var res
        res = factorial(n)
        
        var dummy
        dummy = Ifj.write("Factorial of ")
        dummy = Ifj.write(n)
        dummy = Ifj.write(" is ")
        dummy = Ifj.write(res)
        dummy = Ifj.write("\n")
        
        dummy = Ifj.write("Total recursive steps: ")
        dummy = Ifj.write(__calls)
        dummy = Ifj.write("\n")
    }
}