import "ifj25" for Ifj
class Program {
    static main() {
        var n
        n = Ifj.read_num()
        var result
        result = 1
        while (n > 0) {
            result = result * n
            n = n - 1
        }
        Ifj.write("Vysledek: ")
        Ifj.write(result)
        Ifj.write("\n")
    }
}
