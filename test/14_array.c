//#include <stdio.h>
//#define printf(X) printf("%s\n", (X))
//#define print(X)  printf("%d\n", (X))
int main() {
  int a[10];
  int i;
  a[0] = 1;

  for(i = 1; i < 10; i = i + 1) {
    a[i] = a[i-1] * 2;
    print(a[i]);
  }
}
