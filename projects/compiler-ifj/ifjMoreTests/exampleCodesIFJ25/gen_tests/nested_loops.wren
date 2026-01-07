import "ifj25" for Ifj
class Program {
    static main() {
        var i
        var j
        i = 1
        while (i <= 3) {
            j = 1
            while (j <= 3) {
                Ifj.write(i)
                Ifj.write("x")
                Ifj.write(j)
                Ifj.write("=")
                Ifj.write(i * j)
                Ifj.write(" ")
                j = j + 1
            }
            Ifj.write("\n")
            i = i + 1
        }
    }
}

