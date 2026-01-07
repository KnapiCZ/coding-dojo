// Semantic error: setter redefinition
import "ifj25" for Ifj
class Program {
    static data=(val) {
        __stored = val
    }
    
    static data=(val) {  // redefinition of setter
        __stored = val * 2
    }
    
    static main() {
        data = 10
        Ifj.write(__stored)
        Ifj.write("\n")
    }
}
