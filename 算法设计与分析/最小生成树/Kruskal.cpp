#include <iostream>
#include <algorithm>
using namespace std;
#define re register
#define il inline
#define ll long long
il ll read(){
    ll x=0,f=1;char ch=' ';
    while(ch<'0' || ch>'9'){if(ch=='-')f=-1;ch=getchar();}
    while(ch>='0' && ch<='9'){x=x*10+(ch^48);ch=getchar();}
    return f==1?x:-x;
}
#define maxn 5005
// 连通分量个数
ll cnt;
// 存储一棵树
ll parent[maxn]={0};
// 记录树的“重量”
ll weight[maxn]={0};
il void init(ll n) {
    cnt = n;
    for (ll i = 1; i <= n; i++) {
        parent[i] = i;
        weight[i] = 1;
    }
}
il ll find(ll x) {
    while (parent[x] != x) {
        // 进行路径压缩
        parent[x] = parent[parent[x]];
        x = parent[x];
    }
    return x;
}
il void u(ll p, ll q) {
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

il bool connected(ll p, ll q) {
    return find(p) == find(q);
}

//u和v是节点号 w是边权重
struct Edge
{
	ll u,v,w;
}edge[200005];

ll n,m,ans;
ll counter = 0;
//按边权排序
il bool cmp(Edge a,Edge b)
{
    return a.w<b.w;
}
il void kruskal()
{    
    //将边的权值排序
    sort(edge,edge+m,cmp);
    for(re ll i=0;i<m;i++)
    {
        //若出现两个点已经联通了，则说明这一条边不需要了
        if(connected(edge[i].u,edge[i].v))
        {
            continue;
        }
        //将两个点合并到一个集合
        u(edge[i].u,edge[i].v);  
        //将此边权计入答案
        ans+=edge[i].w;
        counter++;        
        //循环结束条件:边数为点数减一时
        if(counter==n-1)
        {
            break;
        }   
    }
}
int main()
{
    n=read(),m=read();
    //初始化并查集
    init(n);
    for(re ll i=0;i<m;i++)
    {
        edge[i].u=read(),edge[i].v=read(),edge[i].w=read();
    }
    kruskal();
    //图联通的话最后对并查集只有一个连通分量
    if(cnt == 1)
    {
        printf("%lld",ans);
    }
    else
    {
        //图不连通输出orz
        printf("orz\n");
    }
    
    return 0;
}