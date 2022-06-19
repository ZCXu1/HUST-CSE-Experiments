// 最小子图A问题
// 可以解决重边 负边问题
#include <iostream>
#include <cmath>
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
#define inf INT64_MAX
#define maxn 9005
#define maxm 200005
struct edge
{
	ll v,w,next;
}e[maxm<<1];//注意是无向图，开两倍数组
//已经加入最小生成树的点到没有加入的点的最短距离，比如说1和2号节点已经加入了最小生成树，那么dis[3]就等于min(1->3,2->3)
ll head[maxn],dis[maxn],cnt=0,n=0,m=0,tot=0,now=1,ans=0;

bool vis[maxn];

/*
链式前向星加边 
前向星是一种特殊的边集数组,我们把边集数组中的每一条边按照起点从小到大排序,如果起点相同就按照终点从小到大排序,
并记录下以某个点为起点的所有边在数组中的起始位置和存储长度,那么前向星就构造好了.
用head[i]记录以i为起点的最后一条边的储存位置（下标） 通过赋值为cnt可以看出来
*/
il void add(ll u,ll v,ll w)
{
	e[++cnt].v=v;
	e[cnt].w=w;
	e[cnt].next=head[u];
	head[u]=cnt;
}
//读入数据
il void init()
{
    n=read(),m=read();
    for(re ll i=1,u,v,w;i<=m;++i)
    {
        u=read(),v=read(),w=read();
        add(u,v,w),add(v,u,w);
    }
}
il ll prim()
{
	//先把dis数组附为极大值
	for(re ll i=2;i<=n;++i)
	{
		dis[i]=inf;
	}
    //head的下标是节点序号 dis的下标是节点序号 e的下标是边序号
    //初始化以第一个节点为起点的所有边的dis
    //通过head[1] 和e[i].next能找到所有以1号节点为起点的边 且是终点从大到小找的
    //这里要注意重边，所以要用到min
	for(re ll i=head[1];i != 0;i=e[i].next)
	{
		dis[e[i].v]=min(dis[e[i].v],e[i].w);
	}
    while(++tot<n)//最小生成树边数等于点数-1
    {
        re ll curmin=inf;//把minn置为极大值
        vis[now]=1;//标记点已经走过
        //枚举每一个没有使用的点
        //找出最小值作为新边
        //注意这里不是枚举now点的所有连边，而是1~n
        for(re ll i=1;i<=n;++i)
        {
            if(!vis[i]&&curmin>dis[i])
            {
                curmin=dis[i];
				now=i;
            }
        }
        ans+=curmin;
        //枚举now的所有连边，更新dis数组
        for(re ll i=head[now];i != 0;i=e[i].next)
        {
            // 该条边两个顶点 now和v
        	re ll v=e[i].v;
        	if(dis[v]>e[i].w&&!vis[v])
        	{
        		dis[v]=e[i].w;
        	}
		}
    }
    return ans;
}
int main()
{
    init();
    printf("%lld",prim());
    return 0;
}