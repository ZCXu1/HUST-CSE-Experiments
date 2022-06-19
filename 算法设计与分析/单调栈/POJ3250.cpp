#include <iostream>
#include <stack>
#define rep(i,l,r) for(int i=(l);i<=(r);++i)
#define rpe(i,r,l) for(int i=(r);i>=(l);--i)
using namespace std;
typedef long long ll;
inline ll max(ll x,ll y){return x>y?x:y;}
inline ll min(ll x,ll y){return x<y?x:y;}
inline ll read(){
    ll x=0,f=1;char ch=' ';
    while(ch<'0' || ch>'9'){if(ch=='-')f=-1;ch=getchar();}
    while(ch>='0' && ch<='9'){x=x*10+(ch^48);ch=getchar();}
    return f==1?x:-x;
}
stack<int> s;
#define maxN 80005
ll a[maxN]={0};
ll f[maxN]={0};
int main(){
    ll n = read();
    rep(i,1,n){
        a[i] = read();
    }
    ll ans = 0;
    rep(i,1,n){
        while (!s.empty()&&a[s.top()]<=a[i])
        {
            f[s.top()]=i;
            s.pop();
        }
        s.push(i);
    }
    rep(i,1,n-1){
        if(f[i]==0){
            ans+=(n-i);
        }else{
            ans+=(f[i]-i-1);
        }
    }
    cout<<ans<<endl;
    return 0;
}