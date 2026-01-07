import "ifj25" for Ifj
class Program {

    static foo(x, y) {
        var retVal
        retVal = x + y
        return retVal
    }

    static main() {
        var a
        a = 0
        while (a < 4) {
            a = foo(a, 2)
        }
    }
}
