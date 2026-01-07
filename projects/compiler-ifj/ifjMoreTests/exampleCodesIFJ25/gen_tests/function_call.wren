import "ifj25" for Ifj
class Program {
    static add(a, b) {
        return a + b
    }
    
    static multiply(x, y) {
        return x * y
    }
    
    static main() {
        var result1
        result1 = add(10, 20)
        Ifj.write(result1)
        Ifj.write("\n")
        var result2
        result2 = multiply(5, 6)
        Ifj.write(result2)
        Ifj.write("\n")
        var result3
        result3 = add(1, multiply(2, 3))
        Ifj.write(result3)
        Ifj.write("\n")
    }
}

