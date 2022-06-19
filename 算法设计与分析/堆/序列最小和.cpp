#include<iostream>
#include<queue>
#include<vector>
#include<algorithm>
#define rep(i,l,r) for(int i=(l);i<=(r);++i)
#define rpe(i,r,l) for(int i=(r);i>=(l);--i)
typedef long long ll;
using namespace std;
inline ll max(ll x,ll y){return x>y?x:y;}
inline ll read(){
    ll x=0,f=1;char ch=' ';
    while(ch<'0' || ch>'9'){if(ch=='-')f=-1;ch=getchar();}
    while(ch>='0' && ch<='9'){x=x*10+(ch^48);ch=getchar();}
    return f==1?x:-x;
}
#define MAXN 1000005

ll a[MAXN]={0},b[MAXN]={0};
struct Node
{
    ll i;
    ll j;
    Node(ll a,ll b){
        i = a;
        j = b;
    }
    bool operator <(Node n) const  {  return a[i]+b[j] < a[n.i]+b[n.j]; }
    bool operator >(Node n) const  {  return a[i]+b[j] > a[n.i]+b[n.j]; }
};
//greater代表小顶堆
priority_queue<Node,vector<Node>,greater<Node>> q;
int main(){
    ll n =read();

    rep(i,1,n){
        a[i]=read();
    }
    rep(i,1,n){
        b[i]=read();
    }
    sort(a+1,a+n+1);
    sort(b+1,b+n+1);
    rep(i,1,n){
        q.push(Node(i,1));
    }
    vector<ll> v;
    
    while (v.size()<n)
    {
        Node top = q.top();
        q.pop();
        ll i = top.i;
        ll j = top.j;
        v.push_back(a[i]+b[j]);
        q.push(Node(i,j+1));
    }
    for (auto i : v)
    {
        printf("%lld ",i);
    }
    return 0;
}