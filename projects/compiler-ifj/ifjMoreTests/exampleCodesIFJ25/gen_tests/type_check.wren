import "ifj25" for Ifj
class Program {
    static main() {
        var x
        x = Ifj.read_num()
        if (x is Num) {
            Ifj.write("Je to cislo\n")
        } else {
            Ifj.write("Neni to cislo\n")
        }
        var s
        s = "text"
        if (s is String) {
            Ifj.write("Je to retezec\n")
        } else {
            Ifj.write("Neni to retezec\n")
        }
        var n
        n = null
        if (n is Null) {
            Ifj.write("Je to null\n")
        } else {
            Ifj.write("Neni to null\n")
        }
    }
}
