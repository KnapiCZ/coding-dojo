// Semantic error: Ifj.length expects String, got Num literal
import "ifj25" for Ifj
class Program {
    static main() {
        var len 
        len = Ifj.length(42)
        Ifj.write(len)
        Ifj.write("\n")
    }
}
