// Semantic error: Ifj.substring expects Num as second param, got String literal
import "ifj25" for Ifj
class Program {
    static main() {
        var sub 
        sub = Ifj.substring("Hello", "0", 2)
        Ifj.write(sub)
        Ifj.write("\n")
    }
}
