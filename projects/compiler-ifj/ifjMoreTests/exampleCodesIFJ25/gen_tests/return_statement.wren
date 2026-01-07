import "ifj25" for Ifj
class Program {
    static getValue(x) {
        if (x > 0) {
            return x * 2
        } else {
            return 0
        }
    }
    
    static main() {
        var result1
        result1 = getValue(5)
        Ifj.write(result1)
        Ifj.write("\n")
        var result2
        result2 = getValue(-3)
        Ifj.write(result2)
        Ifj.write("\n")
    }
}
