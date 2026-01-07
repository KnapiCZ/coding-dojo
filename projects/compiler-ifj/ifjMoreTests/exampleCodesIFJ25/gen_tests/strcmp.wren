import "ifj25" for Ifj
class Program {
    static main() {
        var s1
        s1 = "abc"
        var s2
        s2 = "abc"
        var result
        result = Ifj.strcmp(s1, s2)
        Ifj.write(result)
    }
}

