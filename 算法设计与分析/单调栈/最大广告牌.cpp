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
ll h[400005]={0};
int main(){
    ll n = read();
    rep(i,1,n){
        h[i] = read();
    }
     //栈记录元素下标，
    stack<int> s;
    ll temp, wide;
    ll ans = 0;
    //next smaller or equal number
    //栈底到栈顶递增
    rep(i,1,n+1) {
        //如果栈空或大于当前栈顶元素
        if (s.empty() || h[i] > h[s.top()]) {
            s.push(i);
        } else if (h[s.top()] > h[i]) {
         //循环计算面积
            while (!s.empty() && (h[s.top()] >= h[i])) {
                temp = s.top();
                s.pop();
                //计算宽度和面积
                wide = s.empty() ? i - 1 : i - s.top() - 1;
                ans = max(ans, h[temp] * wide);
            }
            s.push(i);
        }
    }
    cout << ans;
    return 0;
}