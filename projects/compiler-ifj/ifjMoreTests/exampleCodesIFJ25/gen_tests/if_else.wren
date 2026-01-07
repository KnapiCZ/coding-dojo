import "ifj25" for Ifj
class Program {
    static main() {
        var x
        x = Ifj.read_num()
        if (x > 10) {
            Ifj.write("Vetsi nez 10\n")
        } else {
            Ifj.write("Mensi nebo rovno 10\n")
        }
        if (x == 0) {
            Ifj.write("Je to nula\n")
        } else {
            Ifj.write("Neni to nula\n")
        }
    }
}

