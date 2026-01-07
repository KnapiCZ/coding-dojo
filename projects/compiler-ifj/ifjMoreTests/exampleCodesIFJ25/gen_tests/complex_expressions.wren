import "ifj25" for Ifj
class Program {
    static add(a, b) {
        return a + b
    }
    
    static multiply(x, y) {
        return x * y
    }
    
    static main() {
        var a
        var b
        var c
        var d
        var result
        var str1
        var str2
        
        a = 10
        b = 5
        c = 3
        d = 2
        str1 = "Hello"
        str2 = "World"
        
        // Arithmetic (Supported by base)
        result = a + b * c - d
        Ifj.write(result)
        Ifj.write("\n")
        
        // Parentheses (Supported by base)
        result = (a + b) * (c - d)
        Ifj.write(result)
        Ifj.write("\n")

        // Function Calls
        var tmp1
        var tmp2
        tmp1 = add(a, b) 
        tmp2 = multiply(c, d)
        result = tmp1 * tmp2
        Ifj.write(result)
        Ifj.write("\n")

        // Logic using nested IFs instead of && / ||
        // Original: if (a + b > c * d && a - b < c + d)
        var cond1
        var cond2
        // We construct the condition manually
        if (a + b > c * d) {
            if (a - b < c + d) {
                Ifj.write("Comparison 1: true\n")
            } else {
                Ifj.write("Comparison 1: false\n")
            }
        } else {
            Ifj.write("Comparison 1: false\n")
        }

        // Comparison 2: (a > b) || (c < d)
        // Rewrite as: if (a > b) true else if (c < d) true else false
        if (a > b) {
            Ifj.write("Comparison 2: true\n")
        } else {
            if (c < d) {
                 Ifj.write("Comparison 2: true\n")
            } else {
                 Ifj.write("Comparison 2: false\n")
            }
        }

        // Comparison 3: a == b + c && c != d
        // Base scanner usually supports !=
        var rhs
        rhs = b + c
        if (a == rhs) {
            if (c != d) {
                Ifj.write("Comparison 3: true\n")
            } else {
                Ifj.write("Comparison 3: false\n")
            }
        } else {
             Ifj.write("Comparison 3: false\n")
        }

        // String concat
        var strResult
        strResult = str1 + " " + str2
        Ifj.write(strResult)
        Ifj.write("\n")
        
        // Unary minus workaround (0 - a)
        // Replace -a with (0 - a)
        // var zero
        // zero = 0
        var negA
        negA = - a
        var negC
        negC = - c
        result = negA + b * negC
        Ifj.write(result)
        Ifj.write("\n")
    }
}