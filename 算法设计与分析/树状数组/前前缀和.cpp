
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
ll a[100005]={0},c1[100005]={0},c2[100005]={0}; //对应原数组和树状数组
inline ll lowbit(ll x){
    return x&(-x);
}

void add(ll i,ll k){    //在i位置加上k
    ll ax = i*k;
    while(i <= n){
        c1[i] += k;
        c2[i] += ax;
        i += lowbit(i);
    }
}

ll getsum1(ll i){        //求A[1 - i]的和
    ll res = 0;
    while(i > 0){
        res += c1[i];
        i -= lowbit(i);
    }
    return res;
}
ll getsum2(ll i){        //求A[1 - i]的和
    ll res = 0;
    while(i > 0){
        res += c2[i];
        i -= lowbit(i);
    }
    return res;
}

int main(){
    n = read();
    m = read();
    ll sum = 0;
    rep(i,1,n){
        a[i] =read();
        //输入初值的时候，也相当于更新了值
        add(i,a[i]);
    }
    rep(i,1,m){
        string s;
        cin>>s;
        if(s == "Query"){
            ll p = read();
            sum = (p+1)*getsum1(p) - getsum2(p);
            printf("%lld\n",sum);
        }else{
            ll x = read(), y = read();
            ll d = y-a[x];
            a[x] = y;
            add(x,d);
        }
    }
    return 0;
}