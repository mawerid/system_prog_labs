#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// variant 6

int main() {

  // 1 task

  char al = 'F';
  char name[30];
  char surname[30];
  unsigned int num_1 = 356;
  signed int num_2 = -592;
  double num_3 = 1761.98;
  double num_4 = -394.152;
  int num_5 = 206;

  printf("Enter your name: ");
  scanf("%s", name);
  printf("Enter your surname: ");
  scanf("%s", surname);

  printf("'%c';'Меня зовут %s %s';%d;%d;%lf;%lf;%d.\n",
         al,
         name,
         surname,
         num_1,
         num_2,
         num_3,
         num_4,
         num_5);

  // 2 task

  int a, b, c, d, k1, k2;
  int count = 0;

  printf("Enter a b c d:");
  scanf("%d %d %d %d", &a, &b, &c, &d);

  printf("Enter k1 k2:");
  scanf("%d %d", &k1, &k2);

  if (a % k1 != 0 || a % k2 == 0)
    count++;
  if (b % k1 != 0 || b % k2 == 0)
    count++;
  if (c % k1 != 0 || c % k2 == 0)
    count++;
  if (d % k1 != 0 || d % k2 == 0)
    count++;

  printf("Count is %d\n", count);

  // 3 task

  int mass[20];
  int min = 0;

  for (int i = 0; i < 20; i++) {
    mass[i] = rand() % 20;
    if (min < mass[i] && mass[i] % 2 == 1)
      min = mass[i];
  }

  printf("a) %d\n", min);

  int matrix[4][5];
  int sum = 0;

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 5; j++) {
      matrix[i][j] = rand() % 20;
      sum += matrix[i][j];
    }
  }

  printf("b) %d\n", sum);

  // 4 task

  double epsilon = 0.001;
  double x;
  double curr = 1;
  double prev = 0;
  int n = 1;
  double element = 1;

  printf("Enter the X:");
  scanf("%lf", &x);
  while (curr - prev >= epsilon) {
    element *= x / n;
    prev = curr;
    curr += element;
    n++;
  }

  printf("exp(x) = %lf\n", curr);

  // 5 task

  char str[100];
  char s1[10];
  char s2[10];

  printf("Enter the string: ");
  scanf("%s", str);
  size_t str_len = strlen(str);

  printf("Enter s1: ");
  scanf("%s", s1);
  size_t s1_len = strlen(s1);
  printf("Enter s2: ");
  scanf("%s", s2);
  size_t s2_len = strlen(s2);

  printf("Before: %s\n", str);

  char check;
  for (int i = 0; i < str_len; i++) {
    check = 1;
    for (int j = i; j < str_len && (j - i) < s1_len; j++) {
      if (str[j] != s1[j - i]) {
        check = 0;
        break;
      }
    }

    if (check) {
      int j = i;
      if (s1_len > s2_len) {
        strncpy((str + i), s2, s2_len);
        memmove((str + s2_len + i), (str + s1_len + i), (str_len - i - s1_len));
        str_len -= (s1_len - s2_len);
      } else if (s1_len < s2_len) {
        strncpy((str + i), s2, s1_len);
        memmove((str + s2_len + i), (str + s1_len + i), (str_len - i - s1_len));
        strncpy((str + i + s1_len), (s2 + s1_len), (s2_len - s1_len));
        str_len += (s2_len - s1_len);
      } else {
        strncpy((str + i), s2, s2_len);
      }
      i = j;
    }

  }

  printf("After: %s\n", str);

  return 0;
}
