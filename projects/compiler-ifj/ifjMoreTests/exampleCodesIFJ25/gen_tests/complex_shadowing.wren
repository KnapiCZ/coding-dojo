import "ifj25" for Ifj

class Program {
    static main() {
        var x
        x = 10
        Ifj.write("Outer x: ")
        Ifj.write(x) // 10
        Ifj.write("\n")

        if (x > 5) {
            var x
            x = 20
            Ifj.write("Level 1 x: ")
            Ifj.write(x) // 20
            Ifj.write("\n")

            if (x > 15) {
                var x
                x = 30
                Ifj.write("Level 2 x: ")
                Ifj.write(x) // 30
                Ifj.write("\n")
                
                // Modification test
                x = x + 1
                Ifj.write("Level 2 x mod: ")
                Ifj.write(x) // 31
                Ifj.write("\n")
            }

            // Back to Level 1
            Ifj.write("Level 1 x again: ")
            Ifj.write(x) // 20
            Ifj.write("\n")
        }

        // Back to Outer
        Ifj.write("Outer x again: ")
        Ifj.write(x) // 10
        Ifj.write("\n")
    }
}