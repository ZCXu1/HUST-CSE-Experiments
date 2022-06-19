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
#define maxn 100005
// 连通分量个数
ll cnt;
// 存储一棵树
ll parent[maxn]={0};
// 记录树的“重量”
ll weight[maxn]={0};
void init(ll n) {
        cnt = n;
        for (ll i = 1; i <= n; i++) {
            parent[i] = i;
            weight[i] = 1;
        }
    }
ll find(ll x) {
    while (parent[x] != x) {
        // 进行路径压缩
        parent[x] = parent[parent[x]];
        x = parent[x];
    }
    return x;
}

void u(ll p, ll q) {
    ll rootP = find(p);
    ll rootQ = find(q);
    if (rootP == rootQ)
        return;
        
    // 小树接到大树下面，较平衡
    if (weight[rootP] > weight[rootQ]) {
        parent[rootQ] = rootP;
        weight[rootP] += weight[rootQ];
    } else {
        parent[rootP] = rootQ;
        weight[rootQ] += weight[rootP];
    }
    cnt--;
}

ll a[100005][3];
bool cmp(ll a[],ll b[]){
    return a[2]<b[2];
}

int main(){
    ll n = read(),m = read();
    init(n);
    ll **a = new ll*[m+1];
    rep(i,1,m){
        a[i] = new ll[3];
        rep(j,0,2){
            a[i][j] = read();
        }
        
    }
    sort(a+1,a+1+m,cmp);
    rep(i,1,m){
        u(a[i][0],a[i][1]);
        if(cnt == 1){
            printf("%lld\n",a[i][2]);
            return 0;
        }
    }
    printf("-1\n");
    return 0;
}

