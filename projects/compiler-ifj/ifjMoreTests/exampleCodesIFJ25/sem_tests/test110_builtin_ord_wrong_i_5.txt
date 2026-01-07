// Semantic error: Ifj.ord expects Num as second param, got String literal
import "ifj25" for Ifj
class Program {
    static main() {
        var ascii 
        ascii = Ifj.ord("A", "0")
        Ifj.write(ascii)
        Ifj.write("\n")
    }
}
