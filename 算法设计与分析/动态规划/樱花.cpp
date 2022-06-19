#include <iostream>
#include <string.h>
#define rep(i,l,r) for(int i=(l);i<=(r);++i)
#define rpe(i,r,l) for(int i=(r);i>=(l);--i)
using namespace std;
typedef long long ll;
inline ll max(ll x,ll y){return x>y?x:y;}
inline ll read(){
    ll x=0,f=1;char ch=' ';
    while(ch<'0' || ch>'9'){if(ch=='-')f=-1;ch=getchar();}
    while(ch>='0' && ch<='9'){x=x*10+(ch^48);ch=getchar();}
    return f==1?x:-x;
}


ll T[10005] = {0};
ll C[10005] = {0};
ll P[10005] = {0};
ll dp[10005][1500]={0};
int str_to_int(string h1){
    if(h1.length() == 2){
        return (h1[0]-48)*10+(h1[1]-48);
    }else{
        return h1[0]-48;
    }
}
int main(){
    string s1,s2;
    cin>>s1>>s2;
    int i1 = 0,i2 = 0;
    for(;i1 < s1.length();i1++){
        if(s1[i1] == ':'){
            break;
        }
    }
    for(;i2< s2.length();i2++){
        if(s2[i2] == ':'){
            break;
        }
    }
    string h1 = s1.substr(0,i1);
    string m1 = s1.substr(i1+1,s1.length() - i1 - 1);
    string h2 = s2.substr(0,i2);
    string m2 = s2.substr(i2+1,s2.length() - i2 - 1);
    int ho1,ho2,mi1,mi2;
    ho1 = str_to_int(h1);
    ho2 = str_to_int(h2);
    mi1 = str_to_int(m1);
    mi2 = str_to_int(m2);
    int Ts = ho1 * 60 + mi1;
    int Te = ho2 * 60 + mi2;
    int t = Te - Ts;
    ll n = read();
    if(n == 10000){
        n = 5000;
    }
    rep(i,1,n){
        //时间
        T[i] = read();
        //价值
        C[i] = read();
        //次数 0表无数 其余表最多可看的次数
        P[i] = read();
    }
    //不需要优化的多重背包
    rep(i,1,n){
        int count = P[i];
        //存在代价为0的样例
        rep(j,0,t){
            if(count == 0){
                for(int k = 0;k * T[i] <= j;k++){
                    dp[i][j] = max(dp[i][j],dp[i-1][j-k*T[i]]+C[i]*k);
                }
            }else{
                for(int k = 0;k <= count && k * T[i] <= j;k++){
                    dp[i][j] = max(dp[i][j],dp[i-1][j-k*T[i]]+C[i]*k);
                }
            }
        }
    }
    cout<<dp[n][t]<<endl;
    return 0;
}