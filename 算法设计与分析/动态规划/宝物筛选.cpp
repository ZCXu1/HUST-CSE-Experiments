#include <iostream>
#include <vector>
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
ll v[305]={0};
ll w[305]={0};
ll m[305]={0};
ll dp[40005]={0};
struct A{
    ll v,w;
};
vector<A> V;
int main(){
    //1 <= n <= 300
    ll n = read();
    ll W = read();
    //多重背包二进制优化 变成01背包
    rep(i,1,n){
        //价值 重量 件数
        cin>>v[i]>>w[i]>>m[i];
        for(int j = 1;j <= m[i];j <<= 1){
            m[i] -= j;
            V.push_back(A{v[i]*j,w[i]*j});
        }
        if(m[i] > 0){
            V.push_back(A{v[i]*m[i],w[i]*m[i]});
        }
    }
    //01背包的一维dp
    rep(i,0,V.size()-1){
        rpe(j,W,V[i].w){
            dp[j] = max(dp[j],dp[j-V[i].w]+V[i].v);
        }
    }
    cout<<dp[W]<<endl;

    return 0;
}