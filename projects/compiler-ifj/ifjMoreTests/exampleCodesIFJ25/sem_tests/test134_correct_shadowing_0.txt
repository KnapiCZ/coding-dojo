// Correct: variable shadowing in nested scopes
import "ifj25" for Ifj
class Program {
    static main() {
        var x
        x = 10
        __a = Ifj.write(x)
        __a = Ifj.write("\n")
        {
            var x  // shadows outer x (allowed in different scope)
            x = 20
            __a = Ifj.write(x)
            __a = Ifj.write("\n")
        }
        __a = Ifj.write(x)  // outer x still 10
        __a = Ifj.write("\n")
    }
}
