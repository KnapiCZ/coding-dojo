// Correct: globals passed to builtin substring and write
import "ifj25" for Ifj
class Program {
    static main() {
        __str = "HelloWorld"
        __start = 0
        __end = 5
        var sub
        sub = Ifj.substring(__str, __start, __end)
        __a = Ifj.write(sub)
        __a = Ifj.write("\n")
    }
}
