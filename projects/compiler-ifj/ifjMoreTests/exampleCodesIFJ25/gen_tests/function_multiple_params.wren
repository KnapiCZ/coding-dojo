import "ifj25" for Ifj
class Program {
    static addThree(a, b, c) {
        return a + b + c
    }
    
    static multiplyFour(x, y, z, w) {
        return x * y * z * w
    }
    
    static concatStrings(s1, s2, s3) {
        return s1 + s2 + s3
    }
    
    static main() {
        var result1
        result1 = addThree(1, 2, 3)
        Ifj.write(result1)
        Ifj.write("\n")
        
        var result2
        result2 = multiplyFour(2, 3, 4, 5)
        Ifj.write(result2)
        Ifj.write("\n")
        
        var result3
        result3 = concatStrings("Hello", " ", "World")
        Ifj.write(result3)
        Ifj.write("\n")
        
        var result4
        result4 = addThree(10, multiplyFour(1, 2, 3, 4), 5)
        Ifj.write(result4)
        Ifj.write("\n")
    }
}

