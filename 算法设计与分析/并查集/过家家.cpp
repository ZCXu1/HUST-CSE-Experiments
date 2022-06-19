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

bool connected(ll p, ll q) {
    ll rootP = find(p);
    ll rootQ = find(q);
    return rootP == rootQ;
}

bool maps[500][500]; 
// edge1是不吵架对 edge2是女生间朋友对
struct node{
    ll from,to;
}edge1[maxn],edge2[maxn];
//记录每个连通块连接的男生数量
ll num[maxn];
ll ans = INT64_MAX;
/**我们在计算答案的时候,先不考虑k,得到每个连通块能进行的游戏轮数
然后再取最小值,记为ans.
那么考虑k之后的最终答案就是min(ans+k,n)
 */
int main(){
    ll n = read();
    ll m = read();
    ll k = read();
    ll f = read();

    init(n);
    rep(i,1,m){
        ll a = read(),b = read();
        //女from 和 男to 从不吵架 
        edge1[i].from = a;
        edge1[i].to = b;
    }
    rep(i,1,f){
        ll c = read(),d = read();
         //女from 和 女to 是朋友 
        edge2[i].from = c;
        edge2[i].to = d;
    }
    rep(i,1,f){
        //朋友关系用并查集处理联通情况 
        u(edge2[i].from,edge2[i].to);
    }
    rep(i,1,m){
        //要记录每个连通块连接的不同编号的男生数目,用maps标记防止重复计数 
        //遍历edge1 
        if(!maps[find(edge1[i].from)][edge1[i].to]){
             //记录每个连通块的共享男生数目
            num[find(edge1[i].from)]++;
            maps[find(edge1[i].from)][edge1[i].to]=true;
        }
    }
    rep(i,1,m){
        ans=min(ans,num[find(edge1[i].from)]);
        
    }
    ans = min(ans+k,n);
    cout<<ans<<endl;
    return 0;
}