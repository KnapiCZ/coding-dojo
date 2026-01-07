// Semantic error: Ifj.chr expects Num, got String literal
import "ifj25" for Ifj
class Program {
    static main() {
        var ch 
        ch = Ifj.chr("65")
        Ifj.write(ch)
        Ifj.write("\n")
    }
}
