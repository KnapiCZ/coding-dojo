import "ifj25" for Ifj
class Program {
    static main() {
        var a
        var b
        var c
        a = true
        b = false
        c = true
        
        // Test &&
        if (a && c) {
            Ifj.write("a && c is true\n")
        } else {
            Ifj.write("a && c is false\n")
        }
        
        if (a && b) {
            Ifj.write("a && b is true\n")
        } else {
            Ifj.write("a && b is false\n")
        }
        
        // Test ||
        if (a || b) {
            Ifj.write("a || b is true\n")
        } else {
            Ifj.write("a || b is false\n")
        }
        
        if (b || false) {
            Ifj.write("b || false is true\n")
        } else {
            Ifj.write("b || false is false\n")
        }
        
        // Test kombinace
        if ((a && c) || b) {
            Ifj.write("(a && c) || b is true\n")
        } else {
            Ifj.write("(a && c) || b is false\n")
        }
        
        if (a && (b || c)) {
            Ifj.write("a && (b || c) is true\n")
        } else {
            Ifj.write("a && (b || c) is false\n")
        }
    }
}

