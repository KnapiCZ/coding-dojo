import "ifj25" for Ifj
class Program {
    static main() {
        var i
        i = 0
        while(i < 10) {
            i = i + 1
            if(i == 3) {
                continue
            }
            if(i == 7) {
                break
            }
            Ifj.write(i)
            Ifj.write("\n")
        }
    }
}

