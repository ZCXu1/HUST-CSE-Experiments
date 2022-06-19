#include <cmath>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <vector>

using namespace std;

typedef struct Check {
  int A, B, C;
  int flag;
  int sum;
  Check() { A = 0, B = 0, C = 0, flag = 0, sum = 0; }
} check;

int dp[3000005];
char c;
int mon1, mon2;
int main() {
  int Q, N;
  int q1, q2;
  while (1) {
    memset(dp, 0, sizeof(dp));
    scanf("%d.%d %d", &q1, &q2, &N);
    if (N == 0)
      return 0;
    Q = q1 * 100 + q2;
    int n;
    vector<check> ch(N + 1);
    for (int i = 1; i <= N; i++) {
      scanf("%d", &n);
      getchar();
      for (int j = 1; j <= n; j++) {
        scanf("%c:%d.%d", &c, &mon1, &mon2);
        getchar();
        if (c == 'A') {
          ch[i].A += mon1 * 100 + mon2;
          continue;
        }
        if (c == 'B') {
          ch[i].B += mon1 * 100 + mon2;
          continue;
        }
        if (c == 'C') {
          ch[i].C += mon1 * 100 + mon2;
          continue;
        }
        ch[i].flag = 1;
        break;
      }
      ch[i].sum = ch[i].A + ch[i].B + ch[i].C;
      if (ch[i].sum > 100000 || ch[i].A > 60000 || ch[i].B > 60000 ||
          ch[i].C > 60000)
        ch[i].flag = 1;
    }

    for (int i = 1; i <= N; i++) {

      if (ch[i].flag == 1)
        continue;
      for (int j = Q; j >= ch[i].sum; j--) {
        dp[j] = max(dp[j], dp[j - ch[i].sum] + ch[i].sum);
      }
    }

    int res1 = dp[Q] / 100;
    int res2 = dp[Q] % 100;
    printf("%d.%d\n", res1, res2);
  }
  return 0;
}