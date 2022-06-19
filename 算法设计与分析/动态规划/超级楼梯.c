#include <stdio.h>

long long dp[51] = {0};
int main(){
    dp[0] = 1;
    dp[1] = 1;
    for(int j = 2;j < 50;j++){
        dp[j] = dp[j-1]+dp[j-2];
    }
    int n,m;
    scanf("%d",&n);
    for(int i = 0;i < n;i++){
        scanf("%d",&m);
        if(m == 1){
            printf("0\n");
            continue;
        }
        m--;
        printf("%d\n",dp[m]);
    }
    return 0;
}