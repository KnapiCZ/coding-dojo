// Semantic error: Ifj.substring expects String as first param, got Num literal
import "ifj25" for Ifj
class Program {
    static main() {
        var sub 
        sub = Ifj.substring(123, 0, 2)
        Ifj.write(sub)
        Ifj.write("\n")
    }
}
