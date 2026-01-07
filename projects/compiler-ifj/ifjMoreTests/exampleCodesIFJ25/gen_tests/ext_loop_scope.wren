import "ifj25" for Ifj

class Program {
    static main() {
        var i
        i = 999
        
        var sum
        sum = 0
        
        Ifj.write("Outer i before: ")
        Ifj.write(i)
        Ifj.write("\n")
        
        // Loop variable 'i' should shadow outer 'i'
        for (i in 1 .. 3) {
            var temp
            temp = i * 10
            
            Ifj.write("  Inner i: ")
            Ifj.write(i)
            Ifj.write(", temp: ")
            Ifj.write(temp)
            Ifj.write("\n")
            
            // Modify outer sum
            sum = sum + temp
        }
        
        Ifj.write("Outer i after: ")
        Ifj.write(i) // Should still be 999
        Ifj.write("\n")
        
        Ifj.write("Total sum: ")
        Ifj.write(sum) // 10 + 20 + 30 = 60
        Ifj.write("\n")
    }
}