#include <iostream>
#include <stdio.h>

using namespace std;

int Month[13] = {0, 31, 28, 31, 29, 31, 30, 31, 31, 30, 31, 30, 31};

bool isrun(int x) {
  if (x % 100 == 0)
    return x % 400 == 0;
  return x % 4 == 0;
}

int main() {
  int year, mon, day;
  int ans = 0;
  scanf("%d/%d/%d", &year, &mon, &day);
  if (isrun(year) == 1)
    Month[2]++;
  if (mon > 12) {
    mon = mon % 10;
    ans++;
  }
  if (day > Month[mon])
    ans++;
  cout << ans;
  return 0;
}

