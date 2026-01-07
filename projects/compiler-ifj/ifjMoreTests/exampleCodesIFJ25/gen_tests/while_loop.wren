import "ifj25" for Ifj
class Program {
    static main() {
        var n
        n = Ifj.read_num()
        var i
        i = 1
        while (i <= n) {
            Ifj.write(i)
            Ifj.write(" ")
            i = i + 1
        }
        Ifj.write("\n")
    }
}

