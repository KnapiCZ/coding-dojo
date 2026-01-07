import "ifj25" for Ifj

class Program {
    static main() {
        var s1
        s1 = "Hello"
        var s2
        s2 = " World"
        
        // Concatenation
        var res
        res = s1 + s2
        
        var dummy
        dummy = Ifj.write("Original: ")
        dummy = Ifj.write(res)
        dummy = Ifj.write("\n")
        
        // Length
        var len
        len = Ifj.length(res)
        dummy = Ifj.write("Length: ")
        dummy = Ifj.write(len)
        dummy = Ifj.write("\n")
        
        // Substring (Hello)
        var sub
        sub = Ifj.substring(res, 0, 5)
        dummy = Ifj.write("Substring: ")
        dummy = Ifj.write(sub)
        dummy = Ifj.write("\n")
        
        // Ord/Chr
        var code
        code = Ifj.ord(res, 1) // 'e'
        dummy = Ifj.write("ASCII of 'e': ")
        dummy = Ifj.write(code)
        dummy = Ifj.write("\n")
        
        var char
        char = Ifj.chr(code)
        dummy = Ifj.write("Back to char: ")
        dummy = Ifj.write(char)
        dummy = Ifj.write("\n")
        
        // Compare
        var cmp
        cmp = Ifj.strcmp("abc", "abd")
        dummy = Ifj.write("Compare: ")
        dummy = Ifj.write(cmp)
        dummy = Ifj.write("\n")
    }
}