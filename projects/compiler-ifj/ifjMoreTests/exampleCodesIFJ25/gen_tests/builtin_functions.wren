import "ifj25" for Ifj
class Program {
    static main() {
        var num
        num = 3.7
        var floored
        floored = Ifj.floor(num)
        Ifj.write(floored)
        Ifj.write("\n")
        var str
        str = Ifj.str(num)
        Ifj.write(str)
        Ifj.write("\n")
        var text
        text = "Hello"
        var len
        len = Ifj.length(text)
        Ifj.write(len)
        Ifj.write("\n")
    }
}

