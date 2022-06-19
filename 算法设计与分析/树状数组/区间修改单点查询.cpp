//区间更改 单点查询
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
ll a[500005]={0},d[500005]={0}; //对应原数组和树状数组

inline ll lowbit(ll x){
    return x&(-x);
}

void add(ll i,ll k){    //在i位置加上k
    while(i <= n){
        d[i] += k;
        i += lowbit(i);
    }
}

ll getsum(ll i){//求D[1 - i]的和 即A[i]的值
    ll res = 0;
    while(i > 0){
        res += d[i];
        i -= lowbit(i);
    }
    return res;
}

//设 D[i] = A[i] - A[i-1] 则A[i] = sigma(j from 1 to i)D[j]
//则当区间A[X,Y]改变时 D[X+1,Y]不变 只有D[X]和D[Y+1]变
int main(){
    n = read(),m = read();
    rep(i,1,n){
        a[i] = read();
        add(i,a[i]-a[i-1]);
    }
    rep(i,1,m){
        int flag = read();
        if(flag == 1){
            //[X,Y]区间内加上k
            ll x = read(), y = read();
            ll k = read();
            //A[X]-A[X-1]增加k
            add(x,k);
            //A[Y+1]-A[Y]减少k
            add(y+1,-k);
        }else{
            ll x = read();
            printf("%lld\n",getsum(x));
        }
    }
    return 0;
}