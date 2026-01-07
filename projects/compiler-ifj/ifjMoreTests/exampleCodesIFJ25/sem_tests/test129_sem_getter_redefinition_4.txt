// Semantic error: getter redefinition
import "ifj25" for Ifj
class Program {
    static value {
        return 42
    }
    
    static value {  // redefinition of getter
        return 100
    }
    
    static main() {
        var x
        x = value
        Ifj.write(x)
        Ifj.write("\n")
    }
}
