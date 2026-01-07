import "ifj25" for Ifj
class Program {
    static main() {
        var a
        a = 10
        var b
        b = 5
        var c
        c = 2
        var result
        result = (a + b) * c
        Ifj.write(result)
        Ifj.write("\n")
        result = a + b * c
        Ifj.write(result)
        Ifj.write("\n")
        result = (a - b) / c
        Ifj.write(result)
        Ifj.write("\n")
    }
}
