#include <iostream>
#include <cmath>
#include <map>
#include <vector>
using namespace std;
int a[1500]={0};
int b[1500]={0};
int c[1500]={0};
int main(){
    //n个物品总重m
    map<int,vector<int>> map;
    int m,n;
    //共k组
    int k = 0;
    cin>>m>>n;
    for(int i = 1;i <= n;i++){
        //重量 价值 组别
        cin>>a[i]>>b[i]>>c[i];
        k = max(k,c[i]);
        if(map.find(c[i]) == map.end()){
            vector<int> v;
            v.push_back(i);
            map.insert(pair<int,vector<int>>(c[i],v));
        }else{
            map[c[i]].push_back(i);
        }
    }
    //dp[i][j]表示前i组 重量j 最大价值
    vector<vector<int>> dp(k+1,vector<int>(m+1));
    for(int i = 1;i <= k;i++){
        vector<int> v = map[i];
        for(int j = 1;j <= m;j++){
            for(int l : v){
                if(j >= a[l]){
                    dp[i][j] = max(max(dp[i-1][j],dp[i-1][j-a[l]]+b[l]),dp[i][j]);
                }else{
                    dp[i][j] = max(dp[i-1][j],dp[i][j]);
                }
            }
        }
    }
    cout<<dp[k][m]<<endl;

    return 0;
}