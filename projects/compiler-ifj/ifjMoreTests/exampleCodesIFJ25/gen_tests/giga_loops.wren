import "ifj25" for Ifj
class Program {
    static main() {
        var i
        var j
        var k
        var m
        var count
        var outerCount
        var innerCount
        var deepCount
        
        count = 0
        outerCount = 0
        innerCount = 0
        deepCount = 0
        
        // Vnější for loop
        for(i in 1..5) {
            outerCount = outerCount + 1
            Ifj.write("=== Outer for: i=")
            Ifj.write(i)
            Ifj.write(" ===\n")
            
            // První vnitřní while loop
            j = 1
            while(j <= 4) {
                innerCount = innerCount + 1
                Ifj.write("  While j=")
                Ifj.write(j)
                Ifj.write("\n")
                
                // Druhý vnořený for loop
                for(k in 1..3) {
                    deepCount = deepCount + 1
                    Ifj.write("    For k=")
                    Ifj.write(k)
                    Ifj.write(": ")
                    
                    // Třetí vnořený while loop
                    m = 1
                    while(m <= 2) {
                        count = count + 1
                        Ifj.write("m")
                        Ifj.write(m)
                        Ifj.write(" ")
                        
                        // Continue v nejhlubším while
                        if(m == 1 && k == 2 && j == 2) {
                            m = m + 1
                            continue
                        }
                        
                        // Break v nejhlubším while
                        if(m == 1 && k == 1 && j == 3 && i == 2) {
                            break
                        }
                        
                        m = m + 1
                    }
                    
                    Ifj.write("\n")
                    
                    // Continue v for loop (k)
                    if(k == 2 && j == 1 && i == 1) {
                        continue
                    }
                    
                    // Break v for loop (k)
                    if(k == 1 && j == 4 && i == 3) {
                        break
                    }
                }
                
                // Continue v while loop (j)
                if(j == 2 && i == 4) {
                    j = j + 1
                    continue
                }
                
                j = j + 1
                
                // Break v while loop (j)
                if(j == 3 && i == 5) {
                    break
                }
            }
            
            // Třetí vnitřní for loop (nezávislý)
            for(k in 1..2) {
                Ifj.write("  Second for k=")
                Ifj.write(k)
                Ifj.write("\n")
                
                // Vnořený while v druhém for
                m = 1
                while(m <= 2) {
                    Ifj.write("    Nested while m=")
                    Ifj.write(m)
                    Ifj.write("\n")
                    
                    // Break v while uvnitř druhého for
                    if(m == 1 && k == 2 && i == 2) {
                        break
                    }
                    
                    m = m + 1
                }
                
                // Continue v druhém for
                if(k == 1 && i == 3) {
                    continue
                }
            }
            
            // Break v nejvnějším for loop
            if(i == 4) {
                break
            }
        }
        
        Ifj.write("\n=== Statistics ===\n")
        Ifj.write("Total iterations (deepest): ")
        Ifj.write(count)
        Ifj.write("\n")
        Ifj.write("Outer for iterations: ")
        Ifj.write(outerCount)
        Ifj.write("\n")
        Ifj.write("While j iterations: ")
        Ifj.write(innerCount)
        Ifj.write("\n")
        Ifj.write("For k iterations: ")
        Ifj.write(deepCount)
        Ifj.write("\n")
    }
}

