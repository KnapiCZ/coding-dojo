import "ifj25" for Ifj
class Program {
    static main() {
        var text
        text = "Hello World"
        var sub
        sub = Ifj.substring(text, 0, 5)
        Ifj.write(sub)
    }
}

