#include <iostream>
#include <cmath>
using namespace std;
int a[55][3]={0};
//dp[i][j][k]表示前i个食品在体积为j 质量为k的情况下最大卡路里值
int dp[55][405][405]={0};
int main(){
    int v,m,n;
    cin>>v>>m>>n;
    for(int i = 1;i <= n;i++){
        //体积 质量 卡路里
        cin>>a[i][0]>>a[i][1]>>a[i][2];
    }
    for(int i = 1;i <= n;i++){
        //体积
        for(int j = 1;j <= v;j++){
            //质量
            for(int k = 1;k <= m;k++){
                if(j>a[i][0]&&k>a[i][1]){
                    dp[i][j][k] = max(dp[i-1][j][k],dp[i-1][j-a[i][0]][k-a[i][1]]+a[i][2]);
                }else{
                    dp[i][j][k] = dp[i-1][j][k];
                }
            }
        }
    }
    cout<<dp[n][v][m];
    return 0;
}