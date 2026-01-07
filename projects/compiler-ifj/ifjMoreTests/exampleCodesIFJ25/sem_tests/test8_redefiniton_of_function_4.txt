import "ifj25" for Ifj
class Program {
  static foo() {
    return 1
  }
  static foo() {   // redefinice stejné funkce
    return 2
  }

  static main() {
    var x
    x = foo()
  }
} 