//单点修改区间查询
#include<iostream>
#include<queue>
#include<vector>
#include<algorithm>
#define rep(i,l,r) for(ll i=(l);i<=(r);++i)
#define rpe(i,r,l) for(ll i=(r);i>=(l);--i)
typedef long long ll;
using namespace std;
inline ll max(ll x,ll y){return x>y?x:y;}
inline ll read(){
    ll x=0,f=1;char ch=' ';
    while(ch<'0' || ch>'9'){if(ch=='-')f=-1;ch=getchar();}
    while(ch>='0' && ch<='9'){x=x*10+(ch^48);ch=getchar();}
    return f==1?x:-x;
}
ll n,m;
ll a[500005]={0},c[500005]={0}; //对应原数组和树状数组

inline ll lowbit(ll x){
    return x&(-x);
}

void add(ll i,ll k){    //在i位置加上k
    while(i <= n){
        c[i] += k;
        i += lowbit(i);
    }
}

ll getsum(ll i){        //求A[1 - i]的和
    ll res = 0;
    while(i > 0){
        res += c[i];
        i -= lowbit(i);
    }
    return res;
}
int main(){
    n = read();
    m = read();
    rep(i,1,n){
        a[i] =read();
        //输入初值的时候，也相当于更新了值
        add(i,a[i]);
    }
    rep(i,1,m){
        int flag = read();
        if(flag == 1){
            ll x = read();
            ll k = read();
            add(x,k);
        }else{
            ll x = read();
            ll y = read();
            printf("%lld\n",getsum(y)-getsum(x-1));
        }
    }
    return 0;
}