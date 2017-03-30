//#include <stdio.h>
//#define printf(X) printf("%s\n", (X))
//#define print(X)  printf("%d\n", (X))
int main() {
  int a;
  int b;
  int c;
  int d;
  int e;
  a = 1;
  b = 2;
  c = -2;
  d = 54;
  e = 1;
  while (e) {
    c = c + 1;
    b = b - 1;
    if (c > d)
      e = 0;
  }
  print(b);
  print(c);
}
