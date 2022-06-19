//带权并查集模板题
#include<iostream>
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
#define maxn 30005

// 连通分量个数
ll cnt;
// 存储一棵树
ll parent[maxn]={0};
//编号为i的战舰到parent[i]的距离
ll d[maxn] = {0};
//编号为i的战舰所在的一列有多少战舰
ll s[maxn] = {0};
void init(ll n) {
    cnt = n;
    for (ll i = 1; i <= n; i++) {
        parent[i] = i;
        s[i] = 1;
    } 
}
ll find(ll x) {
    if (parent[x] != x) {
        int k = parent[x];
        //每次find的时候都更新d和s
        parent[x] = find(parent[x]);
        d[x] += d[k];
        s[x] = s[parent[x]];
    }
    return parent[x];
}

void u(ll p, ll q) {
    ll rootP = find(p);
    ll rootQ = find(q);
    if (rootP == rootQ)
        return;
    //p放到q的后面
    parent[rootP] = rootQ;

    d[rootP] = d[rootQ] + s[rootQ];
    s[rootQ] += s[rootP];
    s[rootP] = s[rootQ];
    cnt--;
}

bool connected(ll p, ll q) {
    return find(p) == find(q);
}

int main(){
    init(30000);
    ll t = read();
    char c;
    ll a,b;
    rep(i,1,t){
        cin>>c;
        a = read();
        b = read();
        if(c == 'M'){
            u(a,b);
        }else{
            if(connected(a,b)){
                printf("%lld\n",abs(d[a]-d[b])-1);
            }else{
                printf("-1\n");
            }
        }
    }
    return 0;
}