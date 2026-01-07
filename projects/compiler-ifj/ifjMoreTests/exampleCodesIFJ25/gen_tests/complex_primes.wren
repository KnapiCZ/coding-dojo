import "ifj25" for Ifj

class Program {
    static is_prime(n) {
        if (n < 2) { 
            return false 
            }
        if (n == 2) { 
            return true 
            }
        
        // Manual modulo simulation: a % b = a - floor(a/b)*b
        var div
        div = 2
        var max
        max = n / 2
        
        while (div < max + 1) {
            var quotient
            quotient = n / div
            var floored
            floored = Ifj.floor(quotient)
            var rem
            rem = n - (floored * div)

            if (rem == 0) {
                return false
            }
            div = div + 1
        }
        return true
    }

    static main() {
        var count
        count = 0
        var limit
        limit = 20
        
        Ifj.write("Primes up to ")
        Ifj.write(limit)
        Ifj.write(":\n")

        for (i in 1 .. limit) {
            if (is_prime(i)) {
                Ifj.write(i)
                Ifj.write(" ")
                count = count + 1
            }
        }
        Ifj.write("\nTotal found: ")
        Ifj.write(count)
        Ifj.write("\n")
    }
}