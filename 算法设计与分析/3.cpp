#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <bitset>
#define rep(i,l,r) for(ll i=(l);i<=(r);++i)
#define rpe(i,r,l) for(ll i=(r);i>=(l);--i)
#define ll long long
using namespace std;
#define il inline
#define us unsigned short 
#define maxn 100005
il ll read(){
    ll x=0,f=1;char ch=' ';
    while(ch<'0' || ch>'9'){if(ch=='-')f=-1;ch=getchar();}
    while(ch>='0' && ch<='9'){x=x*10+(ch^48);ch=getchar();}
    return f==1?x:-x;
}
struct ip
{
    us b1,b2,b3,b4,type;
};
us getType(ip i){
    if(i.b1 >= 0 && i.b1 <= 127){
        return 1;
    }else if(i.b1 >= 128 && i.b1 <= 191){
        return 2;
    }else if(i.b1 >= 192 && i.b1 <= 223){
        return 3;
    }else if(i.b1 >= 224 && i.b1 <= 239){
        return 4;
    }else{
        return 5;
    }
}
ll calculate(ip i,ll k){
    ll a = (i.b1 << 24)|(i.b2 << 16)|(i.b3 << 8)|(i.b4);
    ll temp = pow(2,32)-pow(2,32-k);
    bitset<32> b1(a);
    bitset<32> b2(temp);
    b1 &= b2;
    b1 >>= (32-k);
    return b1.to_ullong();
}
ip ips[maxn];
ll n,m;
ll k,l,r;
ll cntOfType[6]={0};
int main(){
    n = read(),m = read();
    rep(i,1,n){
        scanf("%d.%d.%d.%d",&ips[i].b1,&ips[i].b2,&ips[i].b3,&ips[i].b4);
        ips[i].type = getType(ips[i]);
    }
    rep(i,1,m){
        k = read(),l = read(),r = read();
        rep(j,1,n){
            ll ans = calculate(ips[j],k);
            if (ans>=l&&ans<=r){
                cntOfType[ips[j].type]++;
            }
        }
        rep(j,1,5){
            if(j < 5){
                printf("%lld ",cntOfType[j]);
            }else{
                printf("%lld\n",cntOfType[j]);
            }
        }
        rep(j,1,5){
            cntOfType[j] = 0;
        }
    }
    return 0;
}