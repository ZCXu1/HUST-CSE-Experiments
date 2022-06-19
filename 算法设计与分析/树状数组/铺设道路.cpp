#include <iostream>
using namespace std;
#define rep(i,l,r) for(ll i=(l);i<=(r);++i)
#define rpe(i,r,l) for(ll i=(r);i>=(l);--i)
typedef long long ll;
inline ll read(){
    ll x=0,f=1;char ch=' ';
    while(ch<'0' || ch>'9'){if(ch=='-')f=-1;ch=getchar();}
    while(ch>='0' && ch<='9'){x=x*10+(ch^48);ch=getchar();}
    return f==1?x:-x;
}
//贪心
ll n = 0,ans = 0;
ll a[100005] = {0};
int main(){
    n = read();
    rep(i,1,n){
        a[i] = read();
    }
    rep(i,1,n){
        ans += a[i]>a[i-1]?(a[i]-a[i-1]):0;
    }
    printf("%lld\n",ans);
    return 0;
}