#include <iostream>
#include <cmath>
using namespace std;
int a[1001][1001] = {0};
int dp[1001][1001] = {0};
int main(){
    int r;
    cin>>r;
    for(int i = 1;i <= r; i++){
        for(int j = 1;j <= i; j++){
            cin>>a[i][j];
        }
    }
    dp[1][1] = a[1][1];
    int ans = dp[1][1];
    for(int i = 2;i <= r;i++){
        for(int j = 1;j <= i;j++){
            if(j == 1){
                dp[i][j] = dp[i-1][j] + a[i][j];
            }else if(j <= r - 1){
                dp[i][j] = max(dp[i-1][j-1],dp[i-1][j]) + a[i][j];
            }else{
                dp[i][j] = dp[i-1][j-1] + a[i][j];
            }
            if(i == r){
                ans = max(ans,dp[i][j]);
            }
        }
    }
    cout<<ans<<endl;

    return 0;
}