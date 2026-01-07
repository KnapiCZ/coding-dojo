import "ifj25" for Ifj
class Program {
    static main() {
        var i
        var j
        var k
        var counter
        
        i = 1
        counter = 0
        
        // Tři while loopy v sobě
        while (i <= 2) {
            j = 1
            while (j <= 2) {
                k = 1
                while (k <= 2) {
                    counter = counter + 1
                    Ifj.write(counter)
                    Ifj.write(": ")
                    Ifj.write(i)
                    Ifj.write(",")
                    Ifj.write(j)
                    Ifj.write(",")
                    Ifj.write(k)
                    Ifj.write("\n")
                    k = k + 1
                }
                j = j + 1
            }
            i = i + 1
        }
        
        Ifj.write("---\n")
        
        // Další příklad se čtyřmi úrovněmi
        var a
        var b
        var c
        var d
        
        a = 1
        while (a <= 2) {
            b = 1
            while (b <= 2) {
                c = 1
                while (c <= 2) {
                    d = 1
                    while (d <= 2) {
                        var sum
                        sum = a + b + c + d
                        Ifj.write(sum)
                        Ifj.write(" ")
                        d = d + 1
                    }
                    c = c + 1
                }
                b = b + 1
            }
            a = a + 1
            Ifj.write("\n")
        }
    }
}

