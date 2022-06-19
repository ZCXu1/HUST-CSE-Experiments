#include<iostream>
#include<algorithm>
#include<cmath>
#define rep(i,l,r) for(ll i=(l);i<=(r);++i)
#define rpe(i,r,l) for(ll i=(r);i>=(l);--i)
#define MAXN 1005
typedef long long ll;
using namespace std;
inline ll max(ll x,ll y){return x>y?x:y;}
inline ll read(){
    ll x=0,f=1;char ch=' ';
    while(ch<'0' || ch>'9'){if(ch=='-')f=-1;ch=getchar();}
    while(ch>='0' && ch<='9'){x=x*10+(ch^48);ch=getchar();}
    return f==1?x:-x;
}

// 并查集部分
// 连通分量个数
ll cnt;
// 存储一棵树
ll parent[MAXN]={0};
// 记录树的“重量”
ll weight[MAXN]={0};
inline void init(ll n) {
    cnt = n;
    for (ll i = 1; i <= n; i++) {
        parent[i] = i;
        weight[i] = 1;
    }
}
inline ll find(ll x) {
    while (parent[x] != x) {
        // 进行路径压缩
        parent[x] = parent[parent[x]];
        x = parent[x];
    }
    return x;
}
inline void u(ll p, ll q) {
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

inline bool connected(ll p, ll q) {
    return find(p) == find(q);
}


ll n,m,x,y;
// nodes[i][0] 代表第i个节点的x坐标 
// nodes[i][1] 代表第i个节点的y坐标
ll nodes[MAXN][2] = {0};

//u和v是节点号 w是边权重 也即边长
struct Edge
{
	ll u,v;
    double w;
    Edge(){}
    Edge(ll _u,ll _v,double _w){u = _u,v = _v,w = _w;}
}edges[500000]; //对于n <= 1000来说 最多有n(n-1)/2 < 500000条边

inline bool cmp(Edge a,Edge b)
{
    return a.w<b.w;
}

int main(){
    n = read(),m = read();
    init(n);
    rep(i,1,n){
        nodes[i][0] = read();
        nodes[i][1] = read();
    }
    ll cntOfEdge = 0;
    double w = 0;
    rep(i,1,n-1){
        rep(j,i+1,n){
            w = sqrt(pow(nodes[i][0]-nodes[j][0],2)+pow(nodes[i][1]-nodes[j][1],2));
            edges[cntOfEdge++] = Edge(i,j,w);
        }
    }
    sort(edges,edges+cntOfEdge,cmp);
    rep(i,1,m){
        x = read(),y = read();
        u(x,y);
    }
    double ans = 0;
    ll counter = 0;
    rep(i,0,cntOfEdge-1){
        if(connected(edges[i].u,edges[i].v)){
            continue;
        }
        u(edges[i].u,edges[i].v); 
        ans += edges[i].w;
        counter++;        
        if(counter==n-1){
            break;
        }  
    }
    printf("%.2f",ans);
    return 0;
}