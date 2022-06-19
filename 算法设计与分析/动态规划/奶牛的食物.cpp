#include<iostream>

typedef long long ll;
using namespace std;
inline ll read(){
    ll x=0,f=1;char ch=' ';
    while(ch<'0' || ch>'9'){if(ch=='-')f=-1;ch=getchar();}
    while(ch>='0' && ch<='9'){x=x*10+(ch^48);ch=getchar();}
    return f==1?x:-x;
}
const int maxn = 4096,M = 100000000;
int n,m;
int tmap[19][19];
int map[19];
int dp[19][maxn];
bool can[maxn];
// 这题也是用二进制来表示状态，先预处理：枚举一行内（不考虑地图因素）每一种状态，看看是否合法，合法的话就打个标记
// 以便后面操作，最后先处理第一行，枚举下面行的时候再枚举一遍上一行，看看两行放置是否合法，合法就累计上一行计数即可。
int main(){
    n = read(),m = read();
    for(int i = 1;i <= n;i++){
        for(int j = 1;j <= m;j++){
            tmap[i][j] = read();
            map[i] = (map[i] << 1) + tmap[i][j];//利用把地图变为二进制的形式可以快速计算是否合法，要对位运算熟悉掌握
        }
    }
    int maxstate = (1 << m) - 1;//最大状态数
    for(int i = 0;i <= maxstate;i++){
        if((((i << 1) & i) == 0) & (((i >> 1) & i) == 0)){
            can[i] = true;
        }
    }
    for(int i = 0;i <= maxstate;i++){
        if((can[i]) & ((i & map[1]) == i)){
                dp[1][i] = 1;//先预处理出第一行（对于某一行的状态，只受上一行影响）
            }
        }
    for(int i = 2;i <= n;i++){
        for(int j = 0;j <= maxstate;j++){
            if((can[j]) & (j & map[i]) == j){
                for(int k = 0;k <= maxstate;k++){
                    if((k & j) == 0){
                        dp[i][j] = (dp[i][j] + dp[i - 1][k]) % M;//dp过程
                        }
                    }
                }
            }
        }
    long long ans = 0;
    for(int i = 0;i <= maxstate;i++){
            ans = (ans + dp[n][i]) % M;//答案在最后一行
        }
    cout<<ans<<endl;
    return 0;
}