// Comprehensive test: All builtin functions with various type scenarios
import "ifj25" for Ifj
class Program {
    static main() {
        __a = Ifj.write("=== Testing Ifj.write with all types ===\n")
        __a = Ifj.write("String: ")
        __a = Ifj.write("Hello World")
        __a = Ifj.write("\n")
        __a = Ifj.write("Integer: ")
        __a = Ifj.write(42)
        __a = Ifj.write("\n")
        __a = Ifj.write("Float: ")
        __a = Ifj.write(3.14)
        __a = Ifj.write("\n")
        __a = Ifj.write("Null: ")
        __a = Ifj.write(null)
        __a = Ifj.write("\n")
        
        __a = Ifj.write("\n=== Testing Ifj.floor ===\n")
        var floored1 
        floored1 = Ifj.floor(42.7)
        __a = Ifj.write("Floor(42.7) = ")
        __a = Ifj.write(floored1)
        __a = Ifj.write("\n")
        var floored2 
        floored2 = Ifj.floor(3.14)
        __a = Ifj.write("Floor(3.14) = ")
        __a = Ifj.write(floored2)
        __a = Ifj.write("\n")
        var floored3 
        floored3 = Ifj.floor(100)
        __a = Ifj.write("Floor(100) = ")
        __a = Ifj.write(floored3)
        __a = Ifj.write("\n")
        
        __a = Ifj.write("\n=== Testing Ifj.str with all types ===\n")
        var str1 
        str1 = Ifj.str(42)
        __a = Ifj.write("str(42) = ")
        __a = Ifj.write(str1)
        __a = Ifj.write("\n")
        var str2 
        str2 = Ifj.str(3.14)
        __a = Ifj.write("str(3.14) = ")
        __a = Ifj.write(str2)
        __a = Ifj.write("\n")
        var str3 
        str3 = Ifj.str("text")
        __a = Ifj.write("str(\"text\") = ")
        __a = Ifj.write(str3)
        __a = Ifj.write("\n")
        var str4 
        str4 = Ifj.str(null)
        __a = Ifj.write("str(null) = ")
        __a = Ifj.write(str4)
        __a = Ifj.write("\n")
        
        __a = Ifj.write("\n=== Testing Ifj.length ===\n")
        var len1 
        len1 = Ifj.length("Hello")
        __a = Ifj.write("length(\"Hello\") = ")
        __a = Ifj.write(len1)
        __a = Ifj.write("\n")
        var len2 
        len2 = Ifj.length("")
        __a = Ifj.write("length(\"\") = ")
        __a = Ifj.write(len2)
        __a = Ifj.write("\n")
        var len3 
        len3 = Ifj.length("Test123")
        __a = Ifj.write("length(\"Test123\") = ")
        __a = Ifj.write(len3)
        __a = Ifj.write("\n")
        
        __a = Ifj.write("\n=== Testing Ifj.substring ===\n")
        var sub1 
        sub1 = Ifj.substring("Hello", 0, 5)
        __a = Ifj.write("substring(\"Hello\", 0, 5) = ")
        __a = Ifj.write(sub1)
        __a = Ifj.write("\n")
        var sub2 
        sub2 = Ifj.substring("Hello", 1, 4)
        __a = Ifj.write("substring(\"Hello\", 1, 4) = ")
        __a = Ifj.write(sub2)
        __a = Ifj.write("\n")
        var sub3 
        sub3 = Ifj.substring("Hello", 0, 2)
        __a = Ifj.write("substring(\"Hello\", 0, 2) = ")
        __a = Ifj.write(sub3)
        __a = Ifj.write("\n")
        var sub4 
        sub4 = Ifj.substring("Test", 2, 2)
        __a = Ifj.write("substring(\"Test\", 2, 2) = ")
        __a = Ifj.write(sub4)
        __a = Ifj.write("\n")
        
        __a = Ifj.write("\n=== Testing Ifj.strcmp ===\n")
        var cmp1 
        cmp1 = Ifj.strcmp("abc", "abc")
        __a = Ifj.write("strcmp(\"abc\", \"abc\") = ")
        __a = Ifj.write(cmp1)
        __a = Ifj.write("\n")
        var cmp2 
        cmp2 = Ifj.strcmp("abc", "def")
        __a = Ifj.write("strcmp(\"abc\", \"def\") = ")
        __a = Ifj.write(cmp2)
        __a = Ifj.write("\n")
        var cmp3 
        cmp3 = Ifj.strcmp("xyz", "abc")
        __a = Ifj.write("strcmp(\"xyz\", \"abc\") = ")
        __a = Ifj.write(cmp3)
        __a = Ifj.write("\n")
        
        __a = Ifj.write("\n=== Testing Ifj.ord ===\n")
        var ord1 
        ord1 = Ifj.ord("A", 0)
        __a = Ifj.write("ord(\"A\", 0) = ")
        __a = Ifj.write(ord1)
        __a = Ifj.write("\n")
        var ord2 
        ord2 = Ifj.ord("Hello", 1)
        __a = Ifj.write("ord(\"Hello\", 1) = ")
        __a = Ifj.write(ord2)
        __a = Ifj.write("\n")
        var ord3 
        ord3 = Ifj.ord("", 0)
        __a = Ifj.write("ord(\"\", 0) = ")
        __a = Ifj.write(ord3)
        __a = Ifj.write("\n")
        var ord4 
        ord4 = Ifj.ord("Test", 10)
        __a = Ifj.write("ord(\"Test\", 10) = ")
        __a = Ifj.write(ord4)
        __a = Ifj.write("\n")
        
        __a = Ifj.write("\n=== Testing Ifj.chr ===\n")
        var chr1 
        chr1 = Ifj.chr(65)
        __a = Ifj.write("chr(65) = ")
        __a = Ifj.write(chr1)
        __a = Ifj.write("\n")
        var chr2 
        chr2 = Ifj.chr(97)
        __a = Ifj.write("chr(97) = ")
        __a = Ifj.write(chr2)
        __a = Ifj.write("\n")
        var chr3 
        chr3 = Ifj.chr(48)
        __a = Ifj.write("chr(48) = ")
        __a = Ifj.write(chr3)
        __a = Ifj.write("\n")
        
        __a = Ifj.write("\n=== All builtin function tests completed ===\n")
    }
}
