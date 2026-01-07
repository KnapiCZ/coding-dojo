import "ifj25" for Ifj
class Program {
    static main() {
        var name
        name = Ifj.read_str()
        var greeting
        greeting = "Ahoj " + name + "!"
        Ifj.write(greeting)
        Ifj.write("\n")
        var repeated
        repeated = "Hi" * 3
        Ifj.write(repeated)
        Ifj.write("\n")
    }
}

