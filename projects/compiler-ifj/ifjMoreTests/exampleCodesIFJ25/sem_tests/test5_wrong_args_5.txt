import "ifj25" for Ifj
class Program {
  static foo(a) {
    return a
  }

  static main() {
    var x
    x = foo(1, 2)  // očekává 1 argument, dostane 2
  }
}