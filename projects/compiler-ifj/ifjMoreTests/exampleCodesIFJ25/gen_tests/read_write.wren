import "ifj25" for Ifj
class Program {
    static main() {
        Ifj.write("Zadej cislo:\n")
        var num
        num = Ifj.read_num()
        Ifj.write("Zadal jsi: ")
        Ifj.write(num)
        Ifj.write("\n")
        Ifj.write("Zadej retezec:\n")
        var str
        str = Ifj.read_str()
        Ifj.write("Zadal jsi: ")
        Ifj.write(str)
        Ifj.write("\n")
    }
}

