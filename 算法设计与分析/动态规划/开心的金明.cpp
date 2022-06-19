#include <iostream>
#include <cmath>
using namespace std;
uint64_t v[100]={0};
uint64_t w[100]={0};
uint64_t vw[100]={0};
//dp[i][j]表示前i个物品在用金钱j内创造的的最大vw
uint64_t dp[100][50000]={0};
int main(){
    int n,m;
    cin>>n>>m;
    for(int i = 1;i <= m;i++){
        cin>>v[i]>>w[i];
        vw[i] = v[i]*w[i];
    }
    for (int i = 1; i <= m; i++){
        for(int j = 1;j <= n;j++){
            if(j < v[i]){
                dp[i][j] = dp[i-1][j];
            }else{
                dp[i][j] = max(dp[i-1][j],dp[i-1][j-v[i]]+vw[i]);
            }
        }
    }
    cout<<dp[m][n]<<endl;
    

    return 0;
}