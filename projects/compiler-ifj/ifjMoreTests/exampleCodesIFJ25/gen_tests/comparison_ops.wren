import "ifj25" for Ifj
class Program {
    static main() {
        var a
        a = Ifj.read_num()
        var b
        b = Ifj.read_num()
        if (a < b) {
            Ifj.write("a < b\n")
        } else {
            Ifj.write("a >= b\n")
        }
        if (a > b) {
            Ifj.write("a > b\n")
        } else {
            Ifj.write("a <= b\n")
        }
        if (a == b) {
            Ifj.write("a == b\n")
        } else {
            Ifj.write("a != b\n")
        }
    }
}

