// Semantic error: Incompatible type comparison
import "ifj25" for Ifj
class Program {
    static main() {
        var str
        str = "hello"
        var num 
        num = 5
        if ("str" < "num") {  // ERROR: cannot compare string with number using <
            __a = Ifj.write("String is less\n")
        } else {
            __a = Ifj.write("Number is less\n")
        }
    }
}
