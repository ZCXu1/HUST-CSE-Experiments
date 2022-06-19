#include <iostream>

using namespace std;
int a[5005] = {0};
int dp[5005] = {0};
long long cnt[5005] = {0};
int main(){
    int n;
    cin>>n;
    for(int i = 1;i <= n;i ++){
        cin>>a[i];
    }
    int maxlen = 0;
    long long ans = 0;
    for(int i = 1;i <= n;i++){
        dp[i] = 1;
        cnt[i] = 1;
        for(int j = 1; j < i;j++){
            if(a[i] < a[j]){
                if(dp[j] + 1 > dp[i]){
                    dp[i] = dp[j] + 1;
                    cnt[i] = cnt[j];
                }else if(dp[j] + 1 == dp[i]){
                    cnt[i] += cnt[j];
                }
            }
        }
        if(dp[i] > maxlen){
            maxlen = dp[i];
            ans = cnt[i];
        }else if(dp[i] == maxlen){
            ans += cnt[i];
        }
    }
    printf("%d %lld",maxlen,ans);

    return 0;
}