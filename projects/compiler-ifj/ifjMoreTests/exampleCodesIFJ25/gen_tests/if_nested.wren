import "ifj25" for Ifj
class Program {
    static main() {
        var x
        x = Ifj.read_num()
        if (x > 0) {
            if (x > 10) {
                Ifj.write("Vetsi nez 10\n")
            } else {
                Ifj.write("Mezi 1 a 10\n")
            }
        } else {
            if (x == 0) {
                Ifj.write("Je to nula\n")
            } else {
                Ifj.write("Zaporne\n")
            }
        }
    }
}

