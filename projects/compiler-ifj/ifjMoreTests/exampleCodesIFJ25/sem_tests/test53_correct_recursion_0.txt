// Correct: Recursive function (fibonacci)
import "ifj25" for Ifj
class Program {
    static fib(n) {
        if (n < 2) {
            return n
        } else {
            var a 
            a = n - 1
            var b 
            b = n - 2
            var fib_a 
            fib_a = fib(a)
            var fib_b 
            fib_b = fib(b)
            return fib_a + fib_b
        }
    }
    
    static main() {
        var n 
        n = 7
        var result 
        result = fib(n)
        __a = Ifj.write("Fibonacci of ")
        __a = Ifj.write(n)
        __a = Ifj.write(" is ")
        __a = Ifj.write(result)
        __a = Ifj.write("\n")
    }
}
