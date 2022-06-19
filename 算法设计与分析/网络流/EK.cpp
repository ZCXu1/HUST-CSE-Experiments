#include <bits/stdc++.h>
using namespace std;
int n,m,s,t,u,v;
long long w,ans,dis[520010];
int tot=1,vis[520010],pre[520010],head[520010],flag[2510][2510];

struct node {
	int to,net;
	long long val;
} e[520010];

inline void add(int u,int v,long long w) {
	e[++tot].to=v;
	e[tot].val=w;
	e[tot].net=head[u];
	head[u]=tot;
	e[++tot].to=u;
	e[tot].val=0;
	e[tot].net=head[v];
	head[v]=tot;
}

inline int bfs() {  //bfs寻找增广路 
	for(register int i=1;i<=n;i++) vis[i]=0;
	queue<int> q;
	q.push(s);
	vis[s]=1;
	dis[s]=2005020600;
	while(!q.empty()) {
		int x=q.front();
		q.pop();
		for(register int i=head[x];i;i=e[i].net) {
			if(e[i].val==0) continue;  //我们只关心剩余流量>0的边 
			int v=e[i].to;
			if(vis[v]==1) continue;  //这一条增广路没有访问过 
			dis[v]=min(dis[x],e[i].val);
			pre[v]=i;  //记录前驱，方便修改边权 
			q.push(v);
			vis[v]=1;
			if(v==t) return 1;  //找到了一条增广路 
		}
	}
	return 0;
}

inline void update() {  //更新所经过边的正向边权以及反向边权 
	int x=t;
	while(x!=s) {
		int v=pre[x];
		e[v].val-=dis[t];
		e[v^1].val+=dis[t];
		x=e[v^1].to;
	}
	ans+=dis[t];   //累加每一条增广路经的最小流量值 
}

int main() {
	scanf("%d%d%d%d",&n,&m,&s,&t);
	for(register int i=1;i<=m;i++) {
		scanf("%d%d%lld",&u,&v,&w);
		if(flag[u][v]==0) {  //处理重边的操作（加上这个模板题就可以用Ek算法过了） 
			add(u,v,w);
			flag[u][v]=tot;
		}
		else {
			e[flag[u][v]-1].val+=w;
		}
	}
	while(bfs()!=0) {  //直到网络中不存在增广路 
		update();
	}
	printf("%lld",ans);
	return 0;
}