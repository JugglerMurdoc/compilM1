//#include <stdio.h>
//#define printf(X) printf("%s\n", (X))
//#define print(X)  printf("%d\n", (X))
int main() {
  int a;
  int b;
  int c;
  int d;
  a = 0;
  b = 0;
  c = -2;
  d = 54;
  while (a < 10) {
    print(a);
    while (b < 10) {
      print(b);
      b = b + 1;
    }
    a = a + 1;
    b = 0;
  }
}
