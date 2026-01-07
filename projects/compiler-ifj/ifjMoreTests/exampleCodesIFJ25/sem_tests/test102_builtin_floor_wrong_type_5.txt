// Semantic error: Ifj.floor expects Num, got String literal
import "ifj25" for Ifj
class Program {
    static main() {
        var result 
        result = Ifj.floor("42.7")
        __a = Ifj.write(result)
        __a = Ifj.write("\n")
    }
}
