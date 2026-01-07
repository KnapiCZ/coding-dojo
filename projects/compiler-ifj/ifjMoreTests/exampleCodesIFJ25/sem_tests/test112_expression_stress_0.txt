// Comprehensive expression parsing stress test - 100+ expressions
import "ifj25" for Ifj

class Program {
    static main() {
        // === LITERALS (10) ===
        var lit1 
        lit1 = 42
        var lit3 
        lit3 = 3.14
        var lit4 
        lit4 = 0.001
        var lit5 
        lit5 = 2e10
        var lit6 
        lit6 = 2.5e-3
        var lit7 
        lit7 = 0x1A3F
        var lit8 
        lit8 = "hello"
        var lit9 
        lit9 = null
        var lit10 
        lit10 = ""
        
        // === BASIC ARITHMETIC (15) ===
        var ar1 
        ar1 = 1 + 2
        var ar2 
        ar2 = 1 + 2 * 3
        var ar3 
        ar3 = (1 + 2) * 3
        var ar4 
        ar4 = 1 * 2 * 3 * 4
        var ar5 
        ar5 = 1 + 2 - 3 + 4
        var ar6 
        ar6 = 10 / 2
        var ar7 
        ar7 = 100 - 50
        var ar8 
        ar8 = 2 * 3 + 4 * 5
        var ar9 
        ar9 = (10 + 20) / 3
        var ar10 
        ar10 = 1.5 * 2.0
        var ar11 
        ar11 = 0xFF + 0x10
        var ar12 
        ar12 = 1e5 + 2e3
        var ar13 
        ar13 = 3.14 * 2
        var ar14 
        ar14 = 0xFF + 3.14
        var ar15 
        ar15 = 1e2 / 50
        
        // === PARENTHESES NESTING (10) ===
        var p1 
        p1 = (1)
        var p2 
        p2 = ((2))
        var p3 
        p3 = (((3)))
        var p4 
        p4 = ((((4))))
        var p5 
        p5 = (((((5)))))
        var p6 
        p6 = ((((((6))))))
        var p7 
        p7 = (1 + (2 + (3 + 4)))
        var p8 
        p8 = (((1 + 2) + 3) + 4)
        var p9 
        p9 = ((1 * 2) + (3 * 4))
        var p10 
        p10 = (((1 + 2) * 3) - ((4 - 5) / 6))
        
        // === RELATIONAL OPERATORS (15) ===
        var rel1 
        rel1 = 1 < 2
        var rel2 
        rel2 = 1 <= 2
        var rel3 
        rel3 = 1 > 0
        var rel4 
        rel4 = 5 >= 3
        var rel5 
        rel5 = 1 == 1
        var rel6 
        rel6 = 1 != 2
        var rel7 
        rel7 = "a" == "a"
        var rel8 
        rel8 = "a" != "b"
        var rel9 
        rel9 = null == null
        var rel10 
        rel10 = null != 1
        var rel11 
        rel11 = 1.5 < 2.5
        var rel12 
        rel12 = 0xFF > 0x10
        var rel13 
        rel13 = (1 + 2) == 3
        var rel14 
        rel14 = (5 * 2) != 11
        var rel15 
        rel15 = (10 - 5) <= (3 + 2)
        
        // === TYPE TEST (is) (10) ===
        var is1 
        is1 = lit1 is Num
        var is2 
        is2 = lit8 is String
        var is3 
        is3 = lit9 is Null
        var is4 
        is4 = (1 + 2) is Num
        var is5 
        is5 = ("a" + "b") is String
        var is6 
        is6 = ar1 is Num
        var is7 
        is7 = (5 * 3) is Num
        var is8 
        is8 = (1 == 2) is Num
        var is9 
        is9 = ("x" * 3) is String
        var is10 
        is10 = ((1 + 2) * 3) is Num
        
        // === STRING OPERATIONS (10) ===
        var s1 
        s1 = "hello" + "world"
        var s2 
        s2 = "a" * 5
        var s3 
        s3 = "test" * 3
        var s4 
        s4 = "" + ""
        var s5 
        s5 = "" * 5
        var s6 
        s6 = "a" * 1
        var s7 
        s7 = ("a" * 2) + ("b" * 3)
        var s8 
        s8 = "hello" + " " + "world"
        var s9 
        s9 = ("x" + "y") * 2
        var s10 
        s10 = "abc" * 0
        
        // === COMPLEX EXPRESSIONS (15) ===
        var c1 
        c1 = (1 + 2) * (3 + 4)
        var c2 
        c2 = ((1 + 2) * 3) + ((4 - 5) * 6)
        var c3 
        c3 = (1 * 2) + (3 / 4) - (5 - 6)
        var c4 
        c4 = ((10 + 20) * 30) / ((40 - 50) + 60)
        var c7 
        c7 = (("a" + "b") * 2) + (("c" + "d") * 3)
        var c9 
        c9 = ((10 / 2) + (20 * 3)) - ((30 - 40) + 50)
        var c10 
        c10 = (((1 + 2) * (3 + 4)) / ((5 - 6) + (7 * 8)))
        var c12 
        c12 = (1 + 2 * 3 - 4 / 5) + (6 * 7 - 8 + 9)
        var c13 
        c13 = ((((1)))) + ((((2)))) * ((((3))))
        var c15 
        c15 = ((1 + 2) * 3) == ((4 - 5) + 6)
        
        // === MIXED TYPES (10) ===
        var m1 
        m1 = 1 + 2.5
        var m2 
        m2 = 3.14 * 2
        var m3 
        m3 = 0xFF + 3.14
        var m4 
        m4 = 1e2 / 50
        var m5 
        m5 = 1 == 1.0
        var m6 
        m6 = 0 == 0.0
        var m7 
        m7 = (1 + 2.5) * 3
        var m8 
        m8 = (0xFF + 10) / 5.0
        var m9 
        m9 = (1e3 * 2) + 3.14
        var m10 
        m10 = ((1 + 2.5) * (3 + 4.5))
        
        __a = Ifj.write("Expression stress test completed!\n")
        __a = Ifj.write("Total expressions: 110\n")
    }
}
