#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <bitset>
#define rep(i,l,r) for(ll i=(l);i<=(r);++i)
#define rpe(i,r,l) for(ll i=(r);i>=(l);--i)
#define ll long long
#define ull unsigned long long
using namespace std;
ull a,s,m;
ull myPow(ull a,ull n){
    ull ans = 1;
    while(n){
        if(n&1){
            ans = (ans * a)%m;
        }
        a = (a * a) % m;
        n >>= 1;
    }
    return ans % m;
}
int main(){
    scanf("%lld %lld %lld",&a,&s,&m);

}