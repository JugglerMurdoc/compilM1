//#include <stdio.h>
//#define printf(X) printf("%s\n", (X))
//#define print(X)  printf("%d\n", (X))
int main() {
  int a;
  int i;
  a = 0;
  for (i = 0; i < 21; i = i + 1) {
    a = a + 3;
    a = a - 1;
  }
  print(a);
}
