#include <stdio.h>

long long dp[60][60] = {0};
int main(){
    for(int i = 1;i <= 50;i ++){
        dp[i][i] = 1;
        dp[i][i+1] = 1;
        dp[i][i+2] = 2;
    }
    int n;
    scanf("%d",&n);
    for(int i = 0;i < n;i++){
        int a,b;
        scanf("%d%d",&a,&b);
        if(dp[a][b] > 0){
            printf("%lld\n",dp[a][b]);
        }else{
            for(int j = a + 3;j <= b;j ++){
                dp[a][j] = dp[a][j-1] + dp[a][j-2];
            }
            printf("%lld\n",dp[a][b]);
        }
    }
    return 0;
}