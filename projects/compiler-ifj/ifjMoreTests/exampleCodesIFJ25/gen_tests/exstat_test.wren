import "ifj25" for Ifj

class Program {
    // Vraci String nebo Num
    static poly_fun() { 
        var a
        a = Ifj.read_num()
        if (a > 10) {
            return "retezec"
        } else {
            return a
        }
    }

    // Vzdy vraci Num (ale ne nutne celociselny)
    static non_poly_fun() { 
        var b
        b = Ifj.read_num()
        if (b < 20) {
            return b * 2
        } else {
            return b
        }
    }

    static main() {
        // --- Valid Variable Definitions ---
        var poly_var
        poly_var = poly_fun()       // muze byt String nebo Num
        var non_poly_var
        non_poly_var = non_poly_fun() // jednoznacne Num
        
        // --- Valid Expressions ---
        // 10 je Num, poly_var muze byt Num. 
        // Pokud je String, je to chyba az za behu (Error 26), ne pri prekladu.
        var x
        x = 10 + poly_var 
        Ifj.write(x)

        // "ahoj" je String, poly_var muze byt String.
        // Pokud je Num, je to chyba az za behu (Error 26).
        var y
        y = "ahoj" + poly_var 
        Ifj.write(y)

        // Literalni vypocty
        var u
        u = 15 * 10.8 + 49
        var v
        v = "ahoj" + " svete"
        
        // --- Valid Function Calls ---
        // non_poly_var je Num. Pokud to neni int, je to chyba 26 (beh), ne 6 (preklad)
        // Pokud mate pokrocilou STATICAN, muzete detekovat, ze to neni int, ale zadani rika "neni chyba prekladu"
        var res
        res = Ifj.ord("ahoj", non_poly_var) 
        Ifj.write(res)
    }
}