// Semantic error: Ifj.strcmp expects String as first param, got Num literal
import "ifj25" for Ifj
class Program {
    static main() {
        var cmp 
        cmp = Ifj.strcmp(42, "abc")
        Ifj.write(cmp)
        Ifj.write("\n")
    }
}
