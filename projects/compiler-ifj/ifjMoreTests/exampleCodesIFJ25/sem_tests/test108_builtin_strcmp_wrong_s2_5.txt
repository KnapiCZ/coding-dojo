// Semantic error: Ifj.strcmp expects String as second param, got Num literal
import "ifj25" for Ifj
class Program {
    static main() {
        var cmp 
        cmp = Ifj.strcmp("abc", 123)
        Ifj.write(cmp)
        Ifj.write("\n")
    }
}
