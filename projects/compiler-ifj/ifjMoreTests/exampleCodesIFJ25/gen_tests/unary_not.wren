import "ifj25" for Ifj
class Program {
    static main() {
        var a
        var b
        a = true
        b = false
        
        Ifj.write(!a)
        Ifj.write("\n")
        Ifj.write(!b)
        Ifj.write("\n")
        
        if (!a) {
            Ifj.write("Should not print\n")
        } else {
            Ifj.write("Correct: !true is false\n")
        }
        
        if (!b) {
            Ifj.write("Correct: !false is true\n")
        } else {
            Ifj.write("Should not print\n")
        }
    }
}

