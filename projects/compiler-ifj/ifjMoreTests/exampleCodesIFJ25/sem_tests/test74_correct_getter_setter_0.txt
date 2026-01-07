// Correct: Getter and setter usage
import "ifj25" for Ifj
class Program {
    static value {
        if (__stored) {
            return __stored
        } else {
            return 0
        }
    }
    
    static value=(v) {
        __stored = v
    }
    
    static main() {
        value = 42
        __a = Ifj.write("Value is: ")
        __a = Ifj.write(value)
        __a = Ifj.write("\n")
        
        value = 100
        __a = Ifj.write("New value is: ")
        __a = Ifj.write(value)
        __a = Ifj.write("\n")
    }
}
