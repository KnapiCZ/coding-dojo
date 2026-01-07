import "ifj25" for Ifj

class Program {
    static gcd(a, b) {
        while (b != 0) {
            var temp
            temp = b
            
            // Modulo simulation: a % b = a - floor(a/b) * b
            var div
            div = a / b
            var fl
            fl = Ifj.floor(div)
            var mult
            mult = fl * b
            var rem
            rem = a - mult
            
            b = rem
            a = temp
        }
        return a
    }

    static main() {
        var x
        x = 54
        var y
        y = 24
        
        var res
        res = gcd(x, y)
        
        var dummy
        dummy = Ifj.write("GCD of ")
        dummy = Ifj.write(x)
        dummy = Ifj.write(" and ")
        dummy = Ifj.write(y)
        dummy = Ifj.write(" is: ")
        dummy = Ifj.write(res)
        dummy = Ifj.write("\n")
    }
}