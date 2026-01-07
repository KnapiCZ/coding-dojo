import "ifj25" for Ifj

class Program {
    static ackermann(m, n) {
        if (m == 0) {
            return n + 1
        }
        
        if (n == 0) {
            var res
            res = ackermann(m - 1, 1)
            return res
        }

        var inner
        inner = ackermann(m, n - 1)
        var res
        res = ackermann(m - 1, inner)
        return res
    }

    static main() {
        Ifj.write("Start Ackermann(3, 2)...\n")
        // A(3, 2) should be 29
        // This requires many recursive calls
        var res 
        res = ackermann(3, 2)
        
        Ifj.write("Result: ")
        Ifj.write(res)
        Ifj.write("\n")
    }
}