import "ifj25" for Ifj
class Program {
    static main() {
        var i
        var j
        var result
        
        // Dva for loopy v sobě - násobilka
        for(i in 1..3) {
            for(j in 1..3) {
                result = i * j
                Ifj.write(result)
                Ifj.write(" ")
            }
            Ifj.write("\n")
        }
        
        // Další příklad s větším rozsahem
        Ifj.write("---\n")
        for(i in 1..4) {
            for(j in 1..4) {
                result = i + j
                Ifj.write(result)
                Ifj.write(" ")
            }
            Ifj.write("\n")
        }
    }
}
