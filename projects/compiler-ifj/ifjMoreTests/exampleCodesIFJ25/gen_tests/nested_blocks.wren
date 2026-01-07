import "ifj25" for Ifj
class Program {
    static main() {
        var x
        x = 1
        Ifj.write(x)
        Ifj.write("\n")
        {
            var x
            x = 2
            Ifj.write(x)
            Ifj.write("\n")
            {
                var x
                x = 3
                Ifj.write(x)
                Ifj.write("\n")
            }
            Ifj.write(x)
            Ifj.write("\n")
        }
        Ifj.write(x)
        Ifj.write("\n")
    }
}

