import "ifj25" for Ifj

class Program {
    static main() {
        Ifj.write("Start Pattern:\n")
        
        // Outer loop 1 to 5
        for (i in 1 .. 5) {
            Ifj.write(i)
            Ifj.write(": ")
            
            // Inner loop 0 to 9
            for (j in 0 .. 9) {
                // Skip specific numbers using continue
                if (j == i) {
                    continue
                }
                
                // Stop inner loop early using break
                if (j > 4) {
                    break
                }
                
                Ifj.write(j)
            }
            Ifj.write("\n")
        }
        Ifj.write("End\n")
    }
}