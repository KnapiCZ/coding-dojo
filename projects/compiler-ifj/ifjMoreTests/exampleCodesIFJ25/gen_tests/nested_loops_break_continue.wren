import "ifj25" for Ifj
class Program {
    static main() {
        var i
        var j
        var k
        var count
        count = 0
        
        // Vnější for loop
        for(i in 1..4) {
            Ifj.write("Outer: ")
            Ifj.write(i)
            Ifj.write("\n")
            
            // První vnitřní while loop
            j = 1
            while(j <= 3) {
                // Druhý vnořený for loop
                for(k in 1..3) {
                    // Continue přeskočí zbytek této iterace
                    if(k == 2) {
                        continue
                    }
                    
                    count = count + 1
                    Ifj.write("  Inner: i=")
                    Ifj.write(i)
                    Ifj.write(" j=")
                    Ifj.write(j)
                    Ifj.write(" k=")
                    Ifj.write(k)
                    Ifj.write("\n")
                    
                    // Break ukončí vnitřní for loop (k)
                    if(i == 2 && j == 2 && k == 1) {
                        break
                    }
                }
                
                // Continue přeskočí zbytek while loop iterace
                if(j == 2 && i == 3) {
                    j = j + 1
                    continue
                }
                
                j = j + 1
                
                // Break ukončí while loop (j)
                if(i == 4 && j == 2) {
                    break
                }
            }
            
            // Break ukončí vnější for loop
            if(i == 3) {
                break
            }
        }
        
        Ifj.write("Final count: ")
        Ifj.write(count)
        Ifj.write("\n")
    }
}

