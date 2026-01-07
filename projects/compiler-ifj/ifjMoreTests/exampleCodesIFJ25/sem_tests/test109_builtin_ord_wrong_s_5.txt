// Semantic error: Ifj.ord expects String as first param, got Num literal
import "ifj25" for Ifj
class Program {
    static main() {
        var ascii 
        ascii = Ifj.ord(65, 0)
        Ifj.write(ascii)
        Ifj.write("\n")
    }
}
